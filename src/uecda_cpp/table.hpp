#ifndef TABLE_HPP_
#define TABLE_HPP_

#include "uecda_common.hpp"

namespace uecda {
  struct Table {
    Table(uecda::common::CommunicationBody src) {
      this->is_my_turn = src[5][2];
      this->whose_turn = src[5][3];
      this->is_start_of_trick = src[5][4];
      this->is_lock = src[5][7];
      this->is_rev = src[5][6];
      for (int i = 0; i < 5; i++) {
	this->card_quantity_of_players[i] = src[6][i];
        this->is_out[i] = this->card_quantity_of_players[i] == 0;
	this->rank_of_players[i] = src[6][5 + i];
	this->player_num_on_seats[i] = src[6][10 + i];
      }
    }

    bool is_my_turn;                 // 自分のターンか？
    int whose_turn;                  // 誰のターンか(プレイヤ番号)。
    bool is_start_of_trick;          // 場に何もないか？
    bool is_lock;                    // 縛り中？
    bool is_rev;                     // 革命中？
    bool has_passed[5];              // 各プレイヤがパスをしたか(ユーザが自分で更新するメンバ)。
    int card_quantity_of_players[5]; // 各プレイヤのカード枚数。
    bool is_out[5];                  // 各プレイヤがあがったか。
    int rank_of_players[5];          // 各プレイヤの現階級。
    int player_num_on_seats[5];      // 各席のプレイヤの番号。
  };
}

#endif // TABLE_HPP_

