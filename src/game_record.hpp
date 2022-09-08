#ifndef GAME_RECORD_HPP_
#define GAME_RECORD_HPP_

#include <array>
#include <algorithm>

#include "uecda_cpp/cards.hpp"

/* ゲームの時系列的な情報を保持する。
   つまり、あるターンの通信テーブルからのみでは得られない情報 = uecda::Tableの補完。 */
struct GameRecord {
  int last_submitted_player;
  std::array<bool, 5> has_passed;

  bool operator ==(const GameRecord &src) const {
    return last_submitted_player == src.last_submitted_player &&
        std::equal(has_passed.begin(), has_passed.end(), src.has_passed.begin());
  }

  friend std::ostream& operator<<(std::ostream& os, const GameRecord& src) {
    os << "GameRecord" << std::endl;
    os << "  最後に提出したプレイヤ: " << src.last_submitted_player << std::endl;
    os << "  パスしたプレイヤ: ";
    for (unsigned int i = 0; i < src.has_passed.size(); i++) {
      if (src.has_passed.at(i)) {
        os << i << " ";
      }
    }
    os << std::endl;
    return os;
  }
};

#endif // GAME_RECORD_HPP_
