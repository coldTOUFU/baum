#ifndef SEARCH_WINNING_HAND_HPP_
#define SEARCH_WINNING_HAND_HPP_

#include <vector>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/hand_summary.hpp"
#include "game_record.hpp"

bool anyOpponentsHaveNCards(const int n, const uecda::Table& table, const GameRecord& record);

/* trump(切札: 確実に場を流せる)か？ */
bool isTrump(const uecda::Hand& hand, uecda::Table table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents);

/* 必勝手を探索する。あればその手を返し、なければ空の手札を返す。 */
uecda::Hand searchWinningHand(const uecda::Cards& my_cards, const uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents);

#endif // SEARCH_WINNING_HAND_HPP_
