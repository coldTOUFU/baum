#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/table.hpp"
#include "uecda_cpp/uecda_client.hpp"
#include "uecda_cpp/uecda_common.hpp"

#include "monte_carlo_tree/monte_carlo_tree_node.hpp"
#include "monte_carlo_tree/uecda_monte_carlo_tree_node.hpp"

#include "game_record.hpp"
#include "uecda_state.hpp"
#include "search_winning_hand.hpp"
#include "simulate_dealing.hpp"

#include "default_playout_policy.hpp"
#include "snowl_playout_policy.hpp"

using namespace uecda;

Hand selectHand(const int my_playernum, const UECdaState& state, const Cards& cards_of_opponents, const unsigned int random_seed) {
  const GameRecord record{state.getRecord()};
  const Cards my_cards{state.getPlayerCards().at(my_playernum)};
  const Table table{state.getTable()};
  const Hand table_hand{state.getTableHand()};

  // 合法手がなければ、何もしない。
  std::vector<Hand> tmp{};
  Hand::pushLegalHands(my_cards, tmp, table, table_hand);
  if (tmp.size() <= 0) { return {}; }

  /* 自分がパスを出せば上がる状況のとき、パスを出す。 */
  if (record.last_submitted_player == my_playernum &&
      std::count_if(record.has_passed.begin(), record.has_passed.end(), [](bool b) { return b; }) == 4) {
    return {};
  }

  /* 必勝手探索。 */
  Hand submission_hand{searchWinningHand(my_cards, table, record, table_hand, cards_of_opponents)};
  if (!submission_hand.getSummary().is_pass) { return submission_hand; }

  /* 必勝手がなければ、モンテカルロ木探索。 */
  // std::function<Hand(const UECdaState&, XorShift64&)> playoutPolicy = [](const UECdaState& s, XorShift64& r) { return snowlPlayoutPolicy(s, r); };
  // MonteCarloTreeNode<UECdaState, Hand, 5> mctnode = MonteCarloTreeNode<UECdaState, Hand, 5>(state, my_playernum, random_seed, 1.0, playoutPolicy);
  MonteCarloTreeNode<UECdaState, Hand, 5> mctnode = MonteCarloTreeNode<UECdaState, Hand, 5>(state, my_playernum, random_seed);
  return mctnode.search();
}

void writeLog(UECdaState& state, std::ofstream& fstream) {
  fstream << state.getRecord().last_submitted_player << std::endl;
  for (int i = 0; i < 5; i++) {
    fstream << state.getRecord().has_passed.at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;
          
  fstream << state.getTableHand().getCards().toBitcards() << "\t" << state.getTableHand().getJoker().toBitcards() << std::endl;

  fstream << state.getTable().is_my_turn << std::endl;
  fstream << state.getTable().whose_turn << std::endl;
  fstream << state.getTable().is_start_of_trick << std::endl;
  fstream << state.getTable().is_rev << std::endl;
  fstream << state.getTable().is_lock << std::endl;
  for (int i = 0; i < 5; i++) {
    fstream << state.getTable().card_quantity_of_players.at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;
  for (int i = 0; i < 5; i++) {
    fstream << state.getTable().is_out.at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;
  for (int i = 0; i < 5; i++) {
    fstream << state.getTable().rank_of_players.at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;
  for (int i = 0; i < 5; i++) {
    fstream << state.getTable().player_num_on_seats.at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;

  for (int i = 0; i < 5; i++) {
    fstream << state.getPlayerCards().at(i).toBitcards();
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;

  for (int i = 0; i < 5; i++) {
    fstream << state.getNextRanks().at(i);
    if (i < 4) {
      fstream << "\t";
    }
  }
  fstream << std::endl;

  fstream << state.getLastAction().getCards().toBitcards() << "\t" << state.getLastAction().getJoker().toBitcards() << std::endl;
}

int main(int argc, char* argv[]) {
  bool is_collect_mode = false;
  std::ofstream fstream;
  if (argc > 1) {
    std::string arg1 = argv[1];
    /* stateのlogを保存。 */
    if (arg1 == "--write-learning-log") {
      fstream.open("state_log.tsv", std::ios::out);
      is_collect_mode = true;
    }
  }

  bool is_round_end = false;
  bool is_game_end = false;

  /* ゲームに参加 */
  UECdaClient client{"baum"};
  const int my_playernum = client.enterGame();

  /* ラウンドの繰り返し */
  while (!is_game_end) {
    is_round_end = false;
    common::CommunicationBody dealt_body = {};
    common::CommunicationBody table_body = {};

    /* 交換前の手札を受け取る */
    client.receiveMyInitialCards(dealt_body);

    /* 交換 */
    if (dealt_body.at(5).at(0) == 0) {
      std::cerr << "ラウンド開始時ですが、カード交換フラグが立っていません。\n";
      return 1;
    }
    const int qty_to_change = dealt_body.at(5).at(1);
    if (qty_to_change == 0) {
      /* 平民以下なので何もしない。 */
    } else if (qty_to_change <= 2 && qty_to_change > 0) {
      /* 手札を作る */
      const Cards my_cards = Cards(Cards::communicationBody2Cards(dealt_body));
      std::vector<Hand> hands(0);
      Hand::pushHands(my_cards, hands);

      /* 交換するカードを決める */
      std::vector<Hand> submission_hands = select_change_hands(hands);

      /* 提出用配列に着手を移す */
      common::CommunicationBody submission_body1 = {};
      common::CommunicationBody submission_body2 = {};
      submission_hands.at(0).putCards(submission_body1);
      if (qty_to_change >= 2) {
        submission_hands.at(1).putCards(submission_body2);
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 15; j++) {
          if (submission_body2.at(i).at(j) == 1) {
            submission_body1.at(i).at(j) = 1;
          }
        }
      }

      /* 交換用カードを提出 */
      client.sendExchangeCards(submission_body1);
    } else {
      std::cerr << "要求されたカード交換枚数が異常です: " << qty_to_change
                << std::endl;
      return 1;
    }

    /* ラウンドを通じて使う変数のセット */
    int last_playernum {};
    GameRecord record {};
    std::array<int, 5> next_ranks {-1, -1, -1, -1, -1};
    Hand last_table_hand {};
    Hand last_action {};
    Cards rest_cards = Cards::all();

    /* 乱数のシード。相手の手番のときに更新する。 */
    std::random_device seed_gen;
    unsigned int random_seed{seed_gen()};

    /* トリックの繰り返し。 */
    while (!is_round_end) {
      /* 自分の手札を受け取る */
      client.receiveMyCards(dealt_body);
      Cards my_cards = Cards(Cards::communicationBody2Cards(dealt_body));
      rest_cards -= my_cards;
      if (my_cards.hasJoker()) {
        rest_cards.deleteJoker();
      }
      /* 場の情報を取得 */
      Table table = Table(dealt_body);

      /* 場の手を作る */
      Hand table_hand = Hand(Hand::communicationBody2Hand(table_body));
      rest_cards -= table_hand.getCards();
      if (table_hand.getSummary().has_joker) {
        rest_cards.deleteJoker();
      }
      if (table.is_start_of_trick) {
        table_hand = Hand();
      }

      /* プレイヤにカードを分配 */
      std::array<Cards, 5> player_cards;
      simulate_random_dealing(my_playernum, my_cards, player_cards, rest_cards, table, random_seed);

      /* ゲームの状態の更新 */
      for (int i = 0; i < 5; i++) {
        if (table.is_out.at(i) && next_ranks.at(i) == -1) {
          next_ranks.at(i) = *std::max_element(next_ranks.begin(), next_ranks.end()) + 1;
        }
      }
      last_action = (table_hand == last_table_hand) ? Hand() : table_hand;
      if (!table.is_start_of_trick && !last_action.getSummary().is_pass) {
        record.last_submitted_player = last_playernum;
      }
      if (table.is_start_of_trick) {
        record.has_passed = table.is_out;
      } else {
        record.has_passed.at(last_playernum) = (last_action.getSummary().is_pass || table.is_out.at(last_playernum));
      }
      UECdaState state = {
        record,
        table_hand,
        table,
        player_cards,
        next_ranks,
        last_action
      };

      /* 着手 */
      if (table.is_my_turn) {
        /* 方策の学習用にstateを保存しておく。 */
        if (is_collect_mode) {
          writeLog(state, fstream);
        }

        Hand submission_hand{selectHand(my_playernum, state, rest_cards, random_seed)};

        /* 提出用配列に着手を移す */
        common::CommunicationBody submission_body = {};
        submission_hand.putCards(submission_body);

        /* カードを提出 */
        const bool is_submit_accepted = client.sendSubmissionCards(submission_body);
        if (!submission_hand.getSummary().is_pass && !is_submit_accepted) { // パスの場合も不受理判定になるので弾く。
          std::cerr << table;
          std::cerr << table_body;
          std::cerr << submission_hand;
          std::cerr << "提出カードが受理されませんでした。" << std::endl;
        }
      } else {
        /* 他プレイヤのターン。 */
        random_seed = seed_gen(); // シード値を変える。
      }

      /* 前の場札を更新(パスなら現在の場札と同じ) */
      last_table_hand = Hand(Hand::communicationBody2Hand(table_body));
      last_playernum = table.whose_turn;

      /* 場札を受け取る */
      client.receiveTableCards(table_body);

      switch (client.receiveGameFinishState()) {
        case UECdaClient::GAME_FINISH_STATE::kContinue:
          is_round_end = false;
          is_game_end = false;
          break;
        case UECdaClient::GAME_FINISH_STATE::kRoundFinish:
          is_round_end = true;
          is_game_end = false;
          break;
        case UECdaClient::GAME_FINISH_STATE::kGameFinish:
          is_round_end = true;
          is_game_end = true;
          break;
      }
    }
  }

  client.exitGame();
  if (is_collect_mode) {
    fstream.close();
  }
  return 0;
}
