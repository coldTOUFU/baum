#ifndef TABLE_HPP_
#define TABLE_HPP_

#include "uecda_common.hpp"

namespace uecda {
  struct Table {
    Table(uecda::common::CommunicationBody src) {
      this->is_my_turn = src[5][2];
      this->is_start_of_trick = src[5][4];
      this->is_lock = src[5][7];
      this->is_rev = src[5][6];
      for (int i = 0; i < 4; i++) {
        this->is_out[i] = src[6][i] == 0;
      }
    }

    bool is_my_turn;
    bool is_start_of_trick;
    bool is_lock;
    bool is_rev;
    bool has_passed[4]; // 各プレイヤがパスをしたか。
    bool is_out[4]; // 各プレイヤがあがったか。
  };
}

#endif // TABLE_HPP_
