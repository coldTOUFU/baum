#ifndef SIMULATE_DEALING_HPP_
#define SIMULATE_DEALING_HPP_

#include <algorithm>
#include <array>
#include <numeric>
#include <random>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/table.hpp"

void simulate_random_dealing(const int my_playernum, const uecda::Cards my_cards, std::array<uecda::Cards, 5>& player_cards, uecda::Cards rest_cards, const uecda::Table table, const unsigned int random_seed);
#endif // SIMULATE_DEALING_HPP_
