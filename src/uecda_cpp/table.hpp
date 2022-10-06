#ifndef TABLE_HPP_
#define TABLE_HPP_

#include <initializer_list>
#include <iostream>

#include "uecda_common.hpp"

namespace uecda {
  struct Table {
    /* テストと探索時シミュレーション用。 */
    Table(bool arg_is_my_turn,
          int arg_whose_turn,
          bool arg_is_start_of_trick,
          bool arg_is_rev,
          bool arg_is_lock,
          std::array<int, 5> arg_card_quantity_of_players,
          std::array<bool, 5> arg_is_out,
          std::array<int, 5> arg_rank_of_players,
          std::array<int, 5> arg_player_num_on_seats):
        is_my_turn(arg_is_my_turn),
        whose_turn(arg_whose_turn),
        is_start_of_trick(arg_is_start_of_trick),
        is_rev(arg_is_rev),
        is_lock(arg_is_lock),
        card_quantity_of_players(arg_card_quantity_of_players),
        is_out(arg_is_out),
        rank_of_players(arg_rank_of_players),
        player_num_on_seats(arg_player_num_on_seats) {}

    /* 実戦でのサーバからの通信処理用。 */
    Table(uecda::common::CommunicationBody& src) {
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

    void print() const {
      std::cout << *this;
    }

    bool operator ==(const Table& src) const {
      return is_my_turn == src.is_my_turn &&
          whose_turn == src.whose_turn &&
          is_start_of_trick == src.is_start_of_trick &&
          is_rev == src.is_rev &&
          is_lock == src.is_lock &&
          std::equal(card_quantity_of_players.begin(), card_quantity_of_players.end(), src.card_quantity_of_players.begin()) &&
          std::equal(is_out.begin(), is_out.end(), src.is_out.begin()) &&
          std::equal(rank_of_players.begin(), rank_of_players.end(), src.rank_of_players.begin()) &&
          std::equal(player_num_on_seats.begin(), player_num_on_seats.end(), src.player_num_on_seats.begin());
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

    friend std::ostream& operator<<(std::ostream& os, const Table& src) {
      os << "Table" << std::endl;
      os << "  自分のターン？: " << (src.is_my_turn ? "YES" : "NO") << std::endl;
      os << "  誰のターン: " << (src.whose_turn) << std::endl;
      os << "  場に何もない？: " << (src.is_start_of_trick ? "YES" : "NO") << std::endl;
      os << "  革命中？: " << (src.is_rev ? "YES" : "NO") << std::endl;
      os << "  縛り中？: " << (src.is_lock ? "YES" : "NO") << std::endl;
      os << "  各プレイヤの手札枚数: ";
      for (const auto e: src.card_quantity_of_players) {
        os << e << " ";
      }
      os << std::endl;
      os << "  上がったプレイヤ: ";
      for (unsigned int i = 0; i < src.is_out.size(); i++) {
        if (src.is_out.at(i)) {
          os << i << " ";
        }
      }
      os << std::endl;
      os << "  各プレイヤの現階級: ";
      for (const auto e: src.rank_of_players) {
        os << e << " ";
      }
      os << std::endl;
      os << "  各席のプレイヤ番号: ";
      for (const auto e: src.player_num_on_seats) {
        os << e << " ";
      }
      os << std::endl;
      return os;
    }
  };
}

#endif // TABLE_HPP_

