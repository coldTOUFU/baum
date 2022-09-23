#ifndef SEARCH_WINNING_VARIATIONS_HPP_
#define SEARCH_WINNING_VARIATIONS_HPP_

#include <vector>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/hand_summary.hpp"
#include "game_record.hpp"

bool someoneHasNCards(const int n, const uecda::Table& table, const GameRecord& record);

/* trump(切札: 確実に場を流せる)か？ */
bool is_trump(const uecda::Hand& hand, uecda::Table table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents);

/* 切札をvariation(手順)の先頭にくっつけていく。行きがけDFSなので、探索が終わったときのvariationは先頭から時系列順になる。返り値は成否。 */
bool pushTrump(const uecda::Cards& my_cards, const uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents, std::vector<uecda::Hand>& variation);

/* 必勝手順を返す。手は添字順。場を流すときのパスは省略している。必勝手順がない場合は空のベクターを返す。 */
std::vector<uecda::Hand> searchWinningVariation(const uecda::Cards& my_cards, uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents);

#endif // SEARCH_WINNING_VARIATIONS_HPP_
