#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/table.hpp"
#include "uecda_cpp/uecda_client.hpp"
#include "uecda_cpp/uecda_common.hpp"
#include "uecda_cpp/sample_client/select_hand.hpp"
#include "monte_carlo_tree/monte_carlo_tree_node.hpp"
#include "game_record.hpp"
#include "uecda_state.hpp"
#include "simulate_dealing.hpp"

using namespace uecda;

int main(int argc, char* argv[]) {
  bool is_round_end = false;
  bool is_game_end = false;

  /* ゲームに参加 */
  UECdaClient client = UECdaClient();
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
      const Cards my_cards = Cards(dealt_body);
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

    /* トリックの繰り返し。 */
    while (!is_round_end) {
      /* 自分の手札を受け取る */
      client.receiveMyCards(dealt_body);
      Cards my_cards = Cards(dealt_body);
      rest_cards -= my_cards;
      /* 場の情報を取得 */
      Table table = Table(dealt_body);

      /* 着手 */
      if (table.is_my_turn) {
        /* 場の手を作る */
        Hand table_hand;
        if (table.is_start_of_trick) {
          table_hand = Hand();
        } else {
          table_hand = Hand(table_body);
        }
        rest_cards -= table_hand.getCards();
        if (table_hand.getSummary().has_joker) {
          rest_cards.deleteJoker();
        }

        /* プレイヤにカードを分配 */
        std::array<Cards, 5> player_cards;
        simulate_random_dealing(my_playernum, my_cards, player_cards, rest_cards, table);

        /* ゲームの状態の更新 */
        for (int i = 0; i < 5; i++) {
          if (table.is_out.at(i) && next_ranks.at(i) == -1) {
            next_ranks.at(i) = *std::max_element(next_ranks.begin(), next_ranks.end()) + 1;
          }
        }
        last_action = (table_hand == last_table_hand) ? Hand() : table_hand;
        if (!table.is_start_of_trick) {
          record.last_submitted_player = last_playernum;
          record.has_passed.at(last_playernum) = (table_hand.getSummary().is_pass || table.is_out.at(last_playernum));
        }
        UECdaState state = {
          record,
          table_hand,
          table,
          player_cards,
          next_ranks,
          last_action
        };

        /* 着手を決める。 */
        MonteCarloTreeNode<UECdaState, Hand> mctnode = MonteCarloTreeNode<UECdaState, Hand>(state, my_playernum);
        Hand submission_hand = mctnode.search();

        /* 提出用配列に着手を移す */
        common::CommunicationBody submission_body = {};
        submission_hand.putCards(submission_body);

        /* カードを提出 */
        const bool is_submit_accepted = client.sendSubmissionCards(submission_body);
        if (!submission_hand.getSummary().is_pass && !is_submit_accepted) { // パスの場合も不受理判定になるので弾く。
          std::cerr << "提出カードが受理されませんでした。" << std::endl;
        }
      } else {
        /* 他プレイヤのターン時の行動を記述 */
      }

      /* 前の場札を更新(パスなら現在の場札と同じ) */
      last_table_hand = Hand(table_body);

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
  return 0;
}
