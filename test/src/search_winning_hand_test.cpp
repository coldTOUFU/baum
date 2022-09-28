#include <gtest/gtest.h>

#include "../../src/uecda_cpp/cards.hpp"
#include "../../src/uecda_cpp/hand.hpp"
#include "../../src/uecda_cpp/table.hpp"
#include "../../src/uecda_cpp/uecda_common.hpp"
#include "../../src/game_record.hpp"
#include "../../src/uecda_state.hpp"
#include "../../src/search_winning_hand.hpp"

using namespace uecda;

class IsTrumpTest : public ::testing::Test {
 protected:
  void updateSrcState() {
    src_table_ = {
      true,
      src_player_num_,
      src_is_start_of_trick_,
      src_is_rev_,
      src_is_lock_,
      src_card_quantity_of_players_,
      src_is_out_,
      src_rank_of_players_,
      src_player_num_on_seats_
    };

    src_state_ = UECdaState{
      src_record_,
      src_table_hand_,
      src_table_,
      src_player_cards_,
      src_next_ranks_,
      src_last_action_
    };
  }

  int src_player_num_{};
  bool src_is_start_of_trick_{true};
  bool src_is_rev_{};
  bool src_is_lock_{};
  std::array<int, 5> src_card_quantity_of_players_{1, 1, 1, 1, 1};
  std::array<bool, 5> src_is_out_{};
  std::array<int, 5> src_rank_of_players_{0, 1, 2, 3, 4};
  std::array<int, 5> src_player_num_on_seats_{0, 1, 2, 3, 4};
  GameRecord src_record_{4, {}};
  Hand src_table_hand_{};
  std::array<Cards, 5> src_player_cards_{
    Cards(Cards::S3),
    Cards(Cards::S2),
    Cards(Cards::H2),
    Cards(Cards::D2),
    Cards(Cards::C2),
  };
  std::array<int, 5> src_next_ranks_{-1, -1, -1, -1, -1};
  Hand src_last_action_{};
  Table src_table_{
    true,
    src_player_num_,
    src_is_start_of_trick_,
    src_is_rev_,
    src_is_lock_,
    src_card_quantity_of_players_,
    src_is_out_,
    src_rank_of_players_,
    src_player_num_on_seats_
  };
  UECdaState src_state_{
    src_record_,
    src_table_hand_,
    src_table_,
    src_player_cards_,
    src_next_ranks_,
    src_last_action_
  };
  Cards src_cards_of_opponents_{Cards::S2 | Cards::H2 | Cards::D2 | Cards::C2};
};

class SearchWinningHandTest : public IsTrumpTest {
 protected:
  Cards src_my_cards_{Cards::S3};
};

/* そもそも手が違法な場合。 */
TEST_F(IsTrumpTest, Illegal) {
  src_table_hand_ = Hand(Cards::H3, {});
  src_is_start_of_trick_ = false;
  updateSrcState();

  Hand hand{Cards::S3, {}};
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 8切の1枚出し。 */
TEST_F(IsTrumpTest, Single8Giri) {
  src_player_cards_.at(0) = Cards(Cards::S8);
  updateSrcState();

  Hand hand{Cards::S8, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* スぺ3返し。 */
TEST_F(IsTrumpTest, Spade3Gaeshi) {
  src_table_hand_ = Hand({}, Cards::S3);
  src_is_start_of_trick_ = false;
  updateSrcState();

  Hand hand{Cards::S3, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* テーブルに1枚出しジョーカーがある場合のスペード3以外の1出し。 */
TEST_F(IsTrumpTest, SingleJokerOnTable) {
  src_player_cards_.at(0) = Cards(Cards::S4);
  src_table_hand_ = Hand({}, Cards::S3);
  src_is_start_of_trick_ = false;
  updateSrcState();

  Hand hand{Cards::S4, {}};
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強いカードを持っている場合の1枚出し。 */
TEST_F(IsTrumpTest, WeakSingle) {
  Hand hand{Cards::S3, {}};
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強いカードを持っている場合の1枚出し(革命時)。 */
TEST_F(IsTrumpTest, WeakSingleOnRevolution) {
  src_player_cards_ = {Cards(Cards::S4), Cards(Cards::S3), Cards(Cards::H3), Cards(Cards::D3), Cards(Cards::C3)};
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand{Cards::S4, {}};
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手の最強カードと同じ強さの1枚出し。 */
TEST_F(IsTrumpTest, SameStrongSingle) {
  src_player_cards_ = {Cards(Cards::S4), Cards(Cards::H4), Cards(Cards::S3), Cards(Cards::H3), Cards(Cards::D3)};
  src_cards_of_opponents_ = Cards(Cards::H4 | Cards::S3 | Cards::H3 | Cards::D3);
  updateSrcState();

  Hand hand{Cards::S4, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手の最強カードと同じ強さの1枚出し(革命時)。 */
TEST_F(IsTrumpTest, SameStrongSingleOnRevolution) {
  src_player_cards_ = {Cards(Cards::S3), Cards(Cards::H3), Cards(Cards::S4), Cards(Cards::H4), Cards(Cards::D4)};
  src_cards_of_opponents_ = Cards(Cards::H3 | Cards::S4 | Cards::H4 | Cards::D4);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand{Cards::S3, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手のどのカードよりも強いカードを出した場合の1枚出し。 */
TEST_F(IsTrumpTest, StrongestSingle) {
  src_player_cards_ = {Cards(Cards::S4), Cards(Cards::S3), Cards(Cards::H3), Cards(Cards::D3), Cards(Cards::C3)};
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3);
  updateSrcState();

  Hand hand{Cards::S4, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手のどのカードよりも強いカードを出した場合の1枚出し(革命時)。 */
TEST_F(IsTrumpTest, StrongestSingleOnRevolution) {
  src_player_cards_ = {Cards(Cards::S3), Cards(Cards::S4), Cards(Cards::H4), Cards(Cards::D4), Cards(Cards::C4)};
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand{Cards::S3, {}};
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* どの相手も同じ大きさのペアを用意できない場合。 */
TEST_F(IsTrumpTest, LargePair) {
  src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
  src_player_cards_ = {Cards(Cards::S3 | Cards::H3), Cards(Cards::S4), Cards(Cards::H4), Cards(Cards::D4), Cards(Cards::C4)};
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手の最強カードと同じ強さのペア。 */
TEST_F(IsTrumpTest, SameStrongPair) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S2 | Cards::H2),
    Cards(Cards::D2 | Cards::C2),
    Cards(Cards::S3),
    Cards(Cards::H3),
    Cards(Cards::D3)
  };
  src_cards_of_opponents_ = Cards(Cards::D2 | Cards::C2 | Cards::S3 | Cards::H3 | Cards::D3);
  updateSrcState();

  Hand hand(Cards::S2 | Cards::H2, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手の最強カードと同じ強さのペア(革命時)。 */
TEST_F(IsTrumpTest, SameStrongPairOnRevolution) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::D3 | Cards::C3),
    Cards(Cards::S4),
    Cards(Cards::H4),
    Cards(Cards::D4)
  };
  src_cards_of_opponents_ = Cards(Cards::D3 | Cards::C3 | Cards::S4 | Cards::H4 | Cards::D4);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手のどのカードよりも強いカードを出した場合のペア。 */
TEST_F(IsTrumpTest, StrongestPair) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S2 | Cards::H2),
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S5),
    Cards(Cards::H5),
    Cards(Cards::D5)
  };
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::S5 | Cards::H5 | Cards::D5);
  updateSrcState();

  Hand hand(Cards::S2 | Cards::H2, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手のどのカードよりも強いカードを出した場合のペア(革命時)。 */
TEST_F(IsTrumpTest, StrongestPairOnRevolution) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S4 | Cards::H4),
    Cards(Cards::S5),
    Cards(Cards::H5),
    Cards(Cards::D5)
  };
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::H4 | Cards::S5 | Cards::H5 | Cards::D5);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* どの相手も同じ大きさの階段を用意できない場合。 */
TEST_F(IsTrumpTest, LargeSequence) {
  src_card_quantity_of_players_ = {3, 1, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::H6),
    Cards(Cards::D6),
    Cards(Cards::C6)};
  src_cards_of_opponents_ = Cards(Cards::S6 | Cards::H6 | Cards::D6 | Cards::C6);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 階段の最強カードと相手の最強カードの距離的にどの相手も階段を返せない場合。 */
TEST_F(IsTrumpTest, StrongestSequence) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::H3 | Cards::H4 | Cards::H5),
    Cards(Cards::S7),
    Cards(Cards::H7),
    Cards(Cards::D7)};
  src_cards_of_opponents_ = Cards(Cards::H3 | Cards::H4 | Cards::H5 | Cards::S7 | Cards::H7 | Cards::D7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 階段の最強カードと相手の最強カードの距離的にどの相手も階段を返せない場合(革命時)。 */
TEST_F(IsTrumpTest, StrongestSequenceOnRevolution) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::H3 | Cards::H4 | Cards::H5),
    Cards(Cards::S7),
    Cards(Cards::H7),
    Cards(Cards::D7)};
  src_cards_of_opponents_ = Cards(Cards::H3 | Cards::H4 | Cards::H5 | Cards::S7 | Cards::H7 | Cards::D7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 最後の条件分岐まで到達し、相手が合法手を構成できない場合のペア。 */
TEST_F(IsTrumpTest, PairLastBranch) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S4 | Cards::H4),
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::S5 | Cards::S6 | Cards::S7);
  updateSrcState();

  Hand hand(Cards::S4 | Cards::H4, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 最後の条件分岐まで到達し、相手が合法手を構成できない場合のペア(革命時)。 */
TEST_F(IsTrumpTest, PairOnRevolutionLastBranch) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S6 | Cards::H6),
    Cards(Cards::S7 | Cards::H7),
    Cards(Cards::S3),
    Cards(Cards::S4),
    Cards(Cards::S5)
  };
  src_cards_of_opponents_ = Cards(Cards::S7 | Cards::H7 | Cards::S3 | Cards::S4 | Cards::S5);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand(Cards::S6 | Cards::H6, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 最後の条件分岐まで到達し、相手が合法手を構成できない場合の階段。 */
TEST_F(IsTrumpTest, SequenceLastBranch) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::H3 | Cards::H4 | Cards::H5),
    Cards(Cards::S7),
    Cards(Cards::D7),
    Cards(Cards::C7)};
  src_cards_of_opponents_ = Cards(Cards::H3 | Cards::H4 | Cards::H5 | Cards::S7 | Cards::D7 | Cards::C7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 最後の条件分岐まで到達し、相手が合法手を構成できない場合の階段(革命時)。 */
TEST_F(IsTrumpTest, SequenceOnRevolutionLastBranch) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S12 | Cards::S1 | Cards::S2),
    Cards(Cards::H12 | Cards::H1 | Cards::H2),
    Cards(Cards::S3),
    Cards(Cards::H3),
    Cards(Cards::D3)};
  src_is_rev_ = true;
  src_cards_of_opponents_ = Cards(Cards::H12 | Cards::H1 | Cards::H2 | Cards::S3 | Cards::H3 | Cards::D3);
  updateSrcState();

  Hand hand(Cards::S12 | Cards::S1 | Cards::S2, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合のペア。 */
TEST_F(IsTrumpTest, WeakPair) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S4 | Cards::H4),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::H4 | Cards::S5 | Cards::S6 | Cards::S7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合のペア(革命時)。 */
TEST_F(IsTrumpTest, WeakPairOnRevolution) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S4 | Cards::H4),
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::S5 | Cards::S6 | Cards::S7);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand(Cards::S4 | Cards::H4, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合のペア(JOKER有で)。 */
TEST_F(IsTrumpTest, WeakPairWithJoker) {
  src_card_quantity_of_players_ = {2, 2, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::H3),
    Cards(Cards::S4 | Cards::JOKER),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::JOKER | Cards::S5 | Cards::S6 | Cards::S7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合の階段。 */
TEST_F(IsTrumpTest, WeakSequence) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::S6 | Cards::S7 | Cards::S8),
    Cards(Cards::S9),
    Cards(Cards::H9),
    Cards(Cards::D9)};
  src_cards_of_opponents_ = Cards(Cards::S6 | Cards::S7 | Cards::S8 | Cards::S9 | Cards::H9 | Cards::D9);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合の階段(革命時)。 */
TEST_F(IsTrumpTest, WeakSequenceOnRevolution) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S9 | Cards::S10 | Cards::S11),
    Cards(Cards::S6 | Cards::S7 | Cards::S8),
    Cards(Cards::S3),
    Cards(Cards::H3),
    Cards(Cards::D3)};
  src_cards_of_opponents_ = Cards(Cards::S6 | Cards::S7 | Cards::S8 | Cards::S3 | Cards::H3 | Cards::D3);
  src_is_rev_ = true;
  updateSrcState();

  Hand hand(Cards::S9 | Cards::S10 | Cards::S11, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 相手がより強い手を持ち得る場合の階段(JOKER有で)。 */
TEST_F(IsTrumpTest, WeakSequenceWithJoker) {
  src_card_quantity_of_players_ = {3, 3, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::S4 | Cards::S5),
    Cards(Cards::S6 | Cards::S7 | Cards::JOKER),
    Cards(Cards::S9),
    Cards(Cards::H9),
    Cards(Cards::D9)};
  src_cards_of_opponents_ = Cards(Cards::S6 | Cards::S7 | Cards::JOKER | Cards::S9 | Cards::H9 | Cards::D9);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::S4 | Cards::S5, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 革命を起こし、起こした後最強になるペア。 */
TEST_F(IsTrumpTest, RevolutingStrongestPair) {
  src_card_quantity_of_players_ = {4, 4, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3),
    Cards(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4 | Cards::S5 | Cards::S6 | Cards::S7);
  updateSrcState();

  Hand hand(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3, {});
  EXPECT_TRUE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 革命を起こし、起こした後相手がより強い手を持ち得るペア。 */
TEST_F(IsTrumpTest, RevolutingWeakPair) {
  src_card_quantity_of_players_ = {4, 4, 1, 1, 1};
  src_player_cards_ = {
    Cards(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4),
    Cards(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3),
    Cards(Cards::S5),
    Cards(Cards::S6),
    Cards(Cards::S7)
  };
  src_cards_of_opponents_ = Cards(Cards::S3 | Cards::H3 | Cards::D3 | Cards::C3 | Cards::S5 | Cards::S6 | Cards::S7);
  updateSrcState();

  Hand hand(Cards::S4 | Cards::H4 | Cards::D4 | Cards::C4, {});
  EXPECT_FALSE(isTrump(hand, src_table_, src_record_, src_state_.getTableHand(), src_cards_of_opponents_));
}

/* 一手詰め。 */
TEST_F(SearchWinningHandTest, OneMoveToWin) {
  Hand dst_hand{Cards::S3, {}};
  EXPECT_EQ(searchWinningHand(src_my_cards_, src_table_, src_record_, src_table_hand_, src_cards_of_opponents_), dst_hand);
}

/* 二手詰め。 */
TEST_F(SearchWinningHandTest, TwoMoveToWin) {
  src_my_cards_ = {Cards::S3 | Cards::S2};
  src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
  src_player_cards_ = {src_my_cards_, Cards::S1, Cards::H1, Cards::D1, Cards::C1};
  src_cards_of_opponents_ = Cards(Cards::S1 | Cards::H1 | Cards::D1 | Cards::C1);
  Hand dst_hand{Cards::S2, {}};
  EXPECT_EQ(searchWinningHand(src_my_cards_, src_table_, src_record_, src_table_hand_, src_cards_of_opponents_), dst_hand);
}

/* 二手詰め(場にカードがある場合)。 */
TEST_F(SearchWinningHandTest, TwoMoveToWinOnTableHand) {
  src_my_cards_ = {Cards::S3 | Cards::S2};
  src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
  src_player_cards_ = {src_my_cards_, Cards::S1, Cards::H1, Cards::D1, Cards::C1};
  src_cards_of_opponents_ = Cards(Cards::S1 | Cards::H1 | Cards::D1 | Cards::C1);
  src_table_hand_ = Hand(Cards::S12, {});
  src_is_start_of_trick_ = false;
  Hand dst_hand{Cards::S2, {}};
  EXPECT_EQ(searchWinningHand(src_my_cards_, src_table_, src_record_, src_table_hand_, src_cards_of_opponents_), dst_hand);
}

/* 必勝手がない場合。 */
TEST_F(SearchWinningHandTest, NoMoveToWin) {
  src_my_cards_ = {Cards::S3 | Cards::S4};
  src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
  src_player_cards_.at(0) = src_my_cards_;
  Hand dst_hand{};
  EXPECT_EQ(searchWinningHand(src_my_cards_, src_table_, src_record_, src_table_hand_, src_cards_of_opponents_), dst_hand);
}
