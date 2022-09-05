#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <iostream>

#include "uecda_common.hpp"

namespace uecda {
  struct Table {
    Table(uecda::common::CommunicationBody src) {
      this->is_my_turn = src.at(5).at(2);
      this->whose_turn = src.at(5).at(3);
      this->is_start_of_trick = src.at(5).at(4);
      this->is_rev = src.at(5).at(6);
      this->is_lock = src.at(5).at(7);
      for (int i = 0; i < 5; i++) {
        this->card_quantity_of_players.at(i) = src.at(6).at(i);
        this->is_out.at(i) = this->card_quantity_of_players.at(i) == 0;
        this->rank_of_players.at(i) = src.at(6).at(5 + i);
        this->player_num_on_seats.at(i) = src.at(6).at(10 + i);
      }
    }

    void print() {
      std::cout << "自分のターン？: " << (this->is_my_turn ? "YES" : "NO") << std::endl;
      std::cout << "誰のターン: " << (this->whose_turn) << std::endl;
      std::cout << "場に何もない？" << (this->is_start_of_trick ? "YES" : "NO") << std::endl;
      std::cout << "革命中？" << (this->is_rev ? "YES" : "NO") << std::endl;
      std::cout << "縛り中？" << (this->is_lock ? "YES" : "NO") << std::endl;
    }

    bool is_my_turn;                             // 自分のターンか？
    int whose_turn;                              // 誰のターンか(プレイヤ番号)。
    bool is_start_of_trick;                      // 場に何もないか？
    bool is_rev;                                 // 革命中？
    bool is_lock;                                // 縛り中？
    std::array<int, 5> card_quantity_of_players; // 各プレイヤのカード枚数。
    std::array<bool, 5> is_out;                  // 各プレイヤがあがったか。
    std::array<int, 5> rank_of_players;          // 各プレイヤの現階級。
    std::array<int, 5> player_num_on_seats;      // 各席のプレイヤの番号。
  };
}

#endif // TABLE_HPP_

