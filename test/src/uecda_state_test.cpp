#include <gtest/gtest.h>

#include "../../src/uecda_cpp/cards.hpp"
#include "../../src/uecda_cpp/hand.hpp"
#include "../../src/uecda_cpp/table.hpp"
#include "../../src/uecda_cpp/uecda_common.hpp"
#include "../../src/game_record.hpp"
#include "../../src/uecda_state.hpp"

using namespace uecda;

class UECdaStateTest: public ::testing::Test {
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

    src_state_ = UECdaState{src_record_, src_table_hand_, src_table_, src_player_cards_, src_next_ranks_, src_last_action_};
  }

  void updateDstState() {
    dst_table_ = {
      true,
      dst_player_num_,
      dst_is_start_of_trick_,
      dst_is_rev_,
      dst_is_lock_,
      dst_card_quantity_of_players_,
      dst_is_out_,
      dst_rank_of_players_,
      dst_player_num_on_seats_
    };

    dst_state_ = UECdaState{dst_record_, dst_table_hand_, dst_table_, dst_player_cards_, dst_next_ranks_, dst_last_action_};
  }

  int src_player_num_{};
  bool src_is_start_of_trick_{};
  bool src_is_rev_{};
  bool src_is_lock_{};
  std::array<int, 5> src_card_quantity_of_players_{};
  std::array<bool, 5> src_is_out_{};
  std::array<int, 5> src_rank_of_players_{};
  std::array<int, 5> src_player_num_on_seats_{};
  GameRecord src_record_{};
  Hand src_table_hand_{};
  std::array<Cards, 5> src_player_cards_{};
  std::array<int, 5> src_next_ranks_{};
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
  UECdaState src_state_{src_record_, src_table_hand_, src_table_, src_player_cards_, src_next_ranks_, src_last_action_};

  int dst_player_num_{};
  bool dst_is_start_of_trick_{};
  bool dst_is_rev_{};
  bool dst_is_lock_{};
  std::array<int, 5> dst_card_quantity_of_players_{};
  std::array<bool, 5> dst_is_out_{};
  std::array<int, 5> dst_rank_of_players_{};
  std::array<int, 5> dst_player_num_on_seats_{};
  GameRecord dst_record_{};
  Hand dst_table_hand_{};
  std::array<Cards, 5> dst_player_cards_{};
  std::array<int, 5> dst_next_ranks_{};
  Hand dst_last_action_{};
  Table dst_table_ = {
    true,
    dst_player_num_,
    dst_is_start_of_trick_,
    dst_is_rev_,
    dst_is_lock_,
    dst_card_quantity_of_players_,
    dst_is_out_,
    dst_rank_of_players_,
    dst_player_num_on_seats_
  };
  UECdaState dst_state_ = {dst_record_, dst_table_hand_, dst_table_, dst_player_cards_, dst_next_ranks_, dst_last_action_};
};

class NextTest: public UECdaStateTest {
 protected:
   /* 各テストの最初の設定。一番頻度の多そうな値にしている。 */
   void SetUp() override {
    src_player_num_ = 0;
    src_is_start_of_trick_ = false;
    src_is_rev_ = false;
    src_is_lock_ = false;
    src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
    src_is_out_ = {};
    src_rank_of_players_ = {0, 1, 2, 3, 4};
    src_player_num_on_seats_ = {0, 1, 2, 3, 4};
    src_record_ = {4, {}};
    src_table_hand_ = Hand((common::CommunicationBody){0, 1});
    src_player_cards_ = {
      Cards({{{}, {0, 1, 1}}}),
      Cards({0, 0, 0, 1}),
      Cards({{{}, {0, 0, 0, 1}}}),
      Cards({{{}, {}, {0, 0, 0, 1}}}),
      Cards({{{}, {}, {}, {0, 0, 0, 1}}})
    };
    src_next_ranks_ = {-1, -1, -1, -1, -1};
    src_last_action_ = src_table_hand_;
    updateSrcState();

    dst_player_num_ = 1;
    dst_is_start_of_trick_ = false;
    dst_is_rev_ = false;
    dst_is_lock_ = false;
    dst_card_quantity_of_players_ = {1, 1, 1, 1, 1};
    dst_is_out_ = {};
    dst_rank_of_players_ = {0, 1, 2, 3, 4};
    dst_player_num_on_seats_ = {0, 1, 2, 3, 4};
    dst_record_ = {src_player_num_, {}};
    dst_table_hand_ = Hand({{{}, {0, 0, 1}}});
    dst_player_cards_ = {
      Cards((common::CommunicationBody){{{}, {0, 1, 0}}}),
      Cards((common::CommunicationBody){{{0, 0, 0, 1}}}),
      Cards((common::CommunicationBody){{{}, {0, 0, 0, 1}}}),
      Cards((common::CommunicationBody){{{}, {}, {0, 0, 0, 1}}}),
      Cards((common::CommunicationBody){{{}, {}, {}, {0, 0, 0, 1}}})
    };
    dst_next_ranks_ = {-1, -1, -1, -1, -1};
    dst_last_action_ = dst_table_hand_;
    updateDstState();
  }
};

class LegalActionsTest: public UECdaStateTest {
 protected:
   void SetUp() override {
    src_player_num_ = 0;
    src_is_start_of_trick_ = false;
    src_is_rev_ = false;
    src_is_lock_ = false;
    src_card_quantity_of_players_ = {6, 1, 1, 1, 1};
    src_is_out_ = {};
    src_rank_of_players_ = {0, 1, 2, 3, 4};
    src_player_num_on_seats_ = {0, 1, 2, 3, 4};
    src_record_ = {4, {}};
    src_table_hand_ = Hand({{{0, 1}, {0, 1}}});
    src_player_cards_ = {
      Cards({{{0, 0, 1}, {0, 0, 1, 1}, {0, 0, 1, 1}, {0, 0, 1}}}),
      Cards({0, 0, 0, 0, 1}),
      Cards({{{}, {0, 0, 0, 0, 1}}}),
      Cards({{{}, {}, {0, 0, 0, 0, 1}}}),
      Cards({{{}, {}, {}, {0, 0, 0, 0, 1}}})
    };
    src_next_ranks_ = {-1, -1, -1, -1, -1};
    src_last_action_ = src_table_hand_;
    updateSrcState();
  }
};

class IsFinishedTest: public UECdaStateTest {
 protected:
   void SetUp() override {
    src_player_num_ = 0;
    src_is_start_of_trick_ = false;
    src_is_rev_ = false;
    src_is_lock_ = false;
    src_card_quantity_of_players_ = {1, 1, 1, 1, 1};
    src_is_out_ = {};
    src_rank_of_players_ = {0, 1, 2, 3, 4};
    src_player_num_on_seats_ = {0, 1, 2, 3, 4};
    src_record_ = {4, {}};
    src_table_hand_ = Hand((common::CommunicationBody){0, 1});
    src_player_cards_ = {
      Cards({{{}, {0, 0, 0, 1}}}),
      Cards({0, 0, 0, 0, 1}),
      Cards({{{}, {0, 0, 0, 0, 1}}}),
      Cards({{{}, {}, {0, 0, 0, 0, 1}}}),
      Cards({{{}, {}, {}, {0, 0, 0, 0, 1}}})
    };
    src_next_ranks_ = {-1, -1, -1, -1, -1};
    src_last_action_ = src_table_hand_;
    updateSrcState();
  }
};

class GetScoreTest: public UECdaStateTest {
 protected:
   void SetUp() override {
    src_player_num_ = 0;
    src_is_start_of_trick_ = false;
    src_is_rev_ = false;
    src_is_lock_ = false;
    src_card_quantity_of_players_ = {0, 0, 0, 0, 0};
    src_is_out_ = {};
    src_rank_of_players_ = {0, 1, 2, 3, 4};
    src_player_num_on_seats_ = {0, 1, 2, 3, 4};
    src_record_ = {4, {true, true, true, true, true}};
    src_table_hand_ = Hand((common::CommunicationBody){0, 1});
    src_player_cards_ = {};
    src_next_ranks_ = {0, 1, 2, 3, 4};
    src_last_action_ = src_table_hand_;
    updateSrcState();
  }
};

class GetLastActionTest: public UECdaStateTest {
 protected:
  void SetUp() override {
    src_player_num_ = 0;
    src_is_start_of_trick_ = false;
    src_is_rev_ = false;
    src_is_lock_ = false;
    src_card_quantity_of_players_ = {2, 1, 1, 1, 1};
    src_is_out_ = {};
    src_rank_of_players_ = {0, 1, 2, 3, 4};
    src_player_num_on_seats_ = {0, 1, 2, 3, 4};
    src_record_ = {4, {}};
    src_table_hand_ = Hand((common::CommunicationBody){0, 1});
    src_player_cards_ = {
      Cards({{{}, {0, 1, 1}}}),
      Cards({0, 0, 0, 1}),
      Cards({{{}, {0, 0, 0, 1}}}),
      Cards({{{}, {}, {0, 0, 0, 1}}}),
      Cards({{{}, {}, {}, {0, 0, 0, 1}}})
    };
    src_next_ranks_ = {-1, -1, -1, -1, -1};
    src_last_action_ = src_table_hand_;
    updateSrcState();
  }
};

TEST_F(NextTest, Pass) {
  dst_table_hand_ = src_table_hand_;
  dst_card_quantity_of_players_ = src_card_quantity_of_players_;
  dst_record_.last_submitted_player = src_record_.last_submitted_player;
  dst_record_.has_passed = {true};
  dst_player_cards_ = src_player_cards_;
  dst_last_action_ = Hand();
  updateDstState();

  Hand next_action = Hand();

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, PassAtLast) {
  src_record_.last_submitted_player = src_player_num_;
  src_record_.has_passed = {false, true, true, true, true};
  src_last_action_ = Hand();
  updateSrcState();
  
  dst_player_num_ = 0;
  dst_is_start_of_trick_ = true;
  dst_card_quantity_of_players_ = src_card_quantity_of_players_;
  dst_table_hand_ = Hand();
  dst_player_cards_.at(0) = src_player_cards_.at(0);
  dst_last_action_ = Hand();
  updateDstState();

  Hand next_action = Hand();

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, PassAtLastWhenLastSubmittedPlayerIsOut) {
  src_card_quantity_of_players_ = {2, 1, 1, 1, 0};
  src_is_out_ = {false, false, false, false, true};
  src_table_hand_ = Hand();
  src_record_.has_passed = {false, true, true, true, true};
  src_player_cards_.at(4) = {};
  updateSrcState();
  
  dst_player_num_ = src_player_num_;
  dst_is_start_of_trick_ = true;
  dst_card_quantity_of_players_ = src_card_quantity_of_players_;
  dst_is_out_ = src_is_out_;
  dst_table_hand_ = Hand();
  dst_record_.last_submitted_player = src_record_.last_submitted_player;
  dst_record_.has_passed = {false, false, false, false, true};
  dst_player_cards_.at(0) = src_player_cards_.at(0);
  dst_player_cards_.at(4) = src_player_cards_.at(4);
  dst_last_action_ = Hand();
  updateDstState();

  Hand next_action = Hand();

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, IllegalSubmission) {
  dst_card_quantity_of_players_ = src_card_quantity_of_players_;
  dst_table_hand_ = src_table_hand_;
  dst_record_.last_submitted_player = src_record_.last_submitted_player;
  dst_record_.has_passed = {true};
  dst_player_cards_.at(0) = src_player_cards_.at(0);
  dst_last_action_ = Hand();
  updateDstState();

  Hand next_action = Hand({{{}, {1}}});

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, NormalSubmission) {
  Hand next_action = Hand({{{}, {0, 0, 1}}});

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, NormalSubmissionWithJoker) {
  src_player_cards_.at(0) = Cards({{{}, {0, 1}, {}, {}, {1}}});
  updateSrcState();

  dst_table_hand_ = Hand({2}); // uecda::Handの仕様に合わせて、ジョーカーは左端に置いている。
  dst_last_action_ = dst_table_hand_;
  updateDstState();

  Hand next_action = dst_table_hand_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, SubmissionAndGettingOut) {
  src_card_quantity_of_players_ = {1, 1, 1, 1, 1};
  src_player_cards_.at(0) = Cards({{{}, {0, 0, 1}}});
  updateSrcState();

  dst_card_quantity_of_players_ = {0, 1, 1, 1, 1};
  dst_is_out_ = {true};
  dst_record_.has_passed = {true};
  dst_player_cards_.at(0) = Cards();
  dst_next_ranks_ = {0, -1, -1, -1, -1};
  updateDstState();

  Hand next_action = Hand({{{}, {0, 0, 1}}});

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, SubmissionAndGameFinished) {
  src_card_quantity_of_players_ = {1, 1, 0, 0, 0};
  src_is_out_ = {false, false, true, true, true};
  src_record_.has_passed = {false, false, true, true, true};
  src_player_cards_.at(0) = Cards({{{}, {0, 0, 1}}});
  src_player_cards_.at(2) = Cards();
  src_player_cards_.at(3) = Cards();
  src_player_cards_.at(4) = Cards();
  src_next_ranks_ = {-1, -1, 0, 1, 2};
  updateSrcState();

  dst_card_quantity_of_players_ = {0, 1, 0, 0, 0};
  dst_is_out_ = {true, true, true, true, true};
  dst_record_.has_passed = {true, true, true, true, true};
  dst_player_cards_.at(0) = Cards();
  dst_player_cards_.at(2) = Cards();
  dst_player_cards_.at(3) = Cards();
  dst_player_cards_.at(4) = Cards();
  dst_next_ranks_ = {3, 4, 0, 1, 2};
  updateDstState();

  Hand next_action = Hand({{{}, {0, 0, 1}}});

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, SubmissionAndLock) {
  src_player_cards_.at(0) = Cards({{{0, 0, 1}, {0, 1, 0}}});
  updateSrcState();

  dst_is_lock_ = true;
  dst_table_hand_ = Hand({0, 0, 1});
  dst_last_action_ = dst_table_hand_;
  updateDstState();

  Hand next_action = dst_table_hand_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, SubmissionAndRev) {
  src_is_start_of_trick_ = true;
  src_card_quantity_of_players_ = {5, 1, 1, 1, 1};
  src_table_hand_ = Hand();
  src_player_cards_.at(0) = Cards({{{0, 0, 1}, {0, 1, 1}, {0, 0, 1}, {0, 0, 1}}});
  updateSrcState();

  dst_is_rev_ = true;
  dst_table_hand_ = Hand({{{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}});
  dst_last_action_ = dst_table_hand_;
  updateDstState();

  Hand next_action = dst_table_hand_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, Submission8GiriSingle) {
  src_player_cards_.at(0) = Cards({{{}, {0, 1, 0, 0, 0, 0, 1}}});
  updateSrcState();

  dst_player_num_ = src_player_num_;
  dst_is_start_of_trick_ = true;
  dst_table_hand_ = Hand();
  dst_last_action_ = Hand({{{}, {0, 0, 0, 0, 0, 0, 1}}});
  updateDstState();

  Hand next_action = dst_last_action_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, Submission8GiriPair) {
  src_card_quantity_of_players_ = {3, 1, 1, 1, 1};
  src_table_hand_ = Hand({{{0, 1}, {0, 1}}});
  src_player_cards_.at(0) = Cards({{{}, {0, 1, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 1}}});
  updateSrcState();

  dst_player_num_ = src_player_num_;
  dst_is_start_of_trick_ = true;
  dst_table_hand_ = Hand();
  dst_last_action_ = Hand({{{}, {0, 0, 0, 0, 0, 0, 1}, {0, 0, 0, 0, 0, 0, 1}}});
  updateDstState();

  Hand next_action = dst_last_action_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, Submission8GiriSequence) {
  src_card_quantity_of_players_ = {4, 1, 1, 1, 1};
  src_table_hand_ = Hand({0, 1, 1, 1});
  src_player_cards_.at(0) = Cards({{{}, {0, 1, 0, 0, 1, 1, 1}}});
  updateSrcState();

  dst_player_num_ = src_player_num_;
  dst_is_start_of_trick_ = true;
  dst_table_hand_ = Hand();
  dst_last_action_ = Hand({{{}, {0, 0, 0, 0, 1, 1, 1}}});
  updateDstState();

  Hand next_action = dst_last_action_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(NextTest, SubmissionSpade3GaeshiSingle) {
  src_table_hand_ = Hand({2});
  src_player_cards_.at(0) = Cards({{{0, 1}, {0, 1}}});
  updateSrcState();

  dst_player_num_ = src_player_num_;
  dst_is_start_of_trick_ = true;
  dst_table_hand_ = Hand();
  dst_last_action_ = Hand((common::CommunicationBody){0, 1});
  updateDstState();

  Hand next_action = dst_last_action_;

  EXPECT_EQ(src_state_.next(next_action), dst_state_);
}

TEST_F(LegalActionsTest, Normal) {
  std::vector<Hand> result_legal_actions = src_state_.legalActions();

  std::vector<Hand> dst_legal_actions = {{
    Hand({{{0, 0, 1}, {0, 0, 1}}}),
    Hand({{{0, 0, 1}, {}, {0, 0, 1}}}),
    Hand({{{0, 0, 1}, {}, {}, {0, 0, 1}}}),
    Hand({{{}, {0, 0, 1}, {0, 0, 1}}}),
    Hand({{{}, {0, 0, 1}, {}, {0, 0, 1}}}),
    Hand({{{}, {}, {0, 0, 1}, {0, 0, 1}}}),
    Hand({{{}, {0, 0, 0, 1}, {0, 0, 0, 1}}}),
    Hand()
  }};

  EXPECT_EQ(result_legal_actions.size(), dst_legal_actions.size());
  for (const auto& a: dst_legal_actions) {
    EXPECT_NE(std::find(result_legal_actions.begin(), result_legal_actions.end(), a), result_legal_actions.end());
  }
}

TEST_F(IsFinishedTest, AllPlayersAreNotOut) {
  EXPECT_FALSE(src_state_.isFinished());
}

TEST_F(IsFinishedTest, SomePlayersAreOut) {
  src_card_quantity_of_players_ = {1, 0, 0, 1, 0};
  src_is_out_ = {false, true, true, false, true};
  src_record_ = {4, {false, true, true, false, true}};
  src_player_cards_ = {
    Cards({{{}, {0, 0, 0, 1}}}),
    {},
    {},
    Cards({{{}, {}, {0, 0, 0, 0, 1}}}),
    {}
  };
  src_next_ranks_ = {-1, 0, 1, -1, 2};
  updateSrcState();

  EXPECT_FALSE(src_state_.isFinished());
}

TEST_F(IsFinishedTest, AllPlayersAreOut) {
  src_card_quantity_of_players_ = {0, 0, 0, 0, 0};
  src_is_out_ = {true, true, true, true, true};
  src_record_ = {4, {true, true, true, true, true}};
  src_player_cards_ = {};
  src_next_ranks_ = {4, 0, 1, 3, 2};
  updateSrcState();

  EXPECT_TRUE(src_state_.isFinished());
}

TEST_F(GetScoreTest, Daifugo) {
  EXPECT_EQ(src_state_.getScore(0), 5);
}

TEST_F(GetScoreTest, Fugo) {
  EXPECT_EQ(src_state_.getScore(1), 4);
}

TEST_F(GetScoreTest, Heimin) {
  EXPECT_EQ(src_state_.getScore(2), 3);
}

TEST_F(GetScoreTest, Hinmin) {
  EXPECT_EQ(src_state_.getScore(3), 2);
}

TEST_F(GetScoreTest, Daihinmin) {
  EXPECT_EQ(src_state_.getScore(4), 1);
}

TEST_F(GetScoreTest, Undetermined) {
  src_card_quantity_of_players_ = {1, 1, 0, 0, 0};
  src_record_ = {4, {false, false, true, true, true}};
  src_player_cards_ = {{
    Cards({0, 0, 1}),
    Cards({0, 0, 0, 1})
  }};
  src_next_ranks_ = {-1, -1, 0, 1, 2};
  updateSrcState();

  EXPECT_EQ(src_state_.getScore(0), 0);
}

TEST_F(GetLastActionTest, Normal) {
  EXPECT_EQ(src_state_.getLastAction(), Hand((common::CommunicationBody){0, 1}));
}
