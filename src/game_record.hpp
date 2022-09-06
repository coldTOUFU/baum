#ifndef GAME_RECORD_HPP_
#define GAME_RECORD_HPP_

#include <array>

#include "uecda_cpp/cards.hpp"

/* ゲームの時系列的な情報を保持する。
   つまり、あるターンの通信テーブルからのみでは得られない情報 = uecda::Tableの補完。 */
struct GameRecord {
  int last_submitted_player;
  std::array<bool, 5> has_passed;
};

#endif // GAME_RECORD_HPP_
