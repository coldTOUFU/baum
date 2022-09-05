#ifndef GAME_RECORD_HPP_
#define GAME_RECORD_HPP_

#include <algorithm>

#include "uecda_cpp/cards.hpp"

/* ゲームの時系列的な情報を保持する。
   つまり、あるターンの通信テーブルからのみでは得られない情報 = uecda::Tableの補完。 */
struct GameRecord {
  int last_submitted_player;
};

#endif // GAME_RECORD_HPP_
