#ifndef LEARN_PLAYOUT_POLICY_HPP_
#define LEARN_PLAYOUT_POLICY_HPP_

#include <array>
#include <fstream>

#include "../../src/uecda_state.hpp"
#include "../../src/uecda_cpp/hand.hpp"
#include "../../src/monte_carlo_tree/monte_carlo_tree_node.hpp"
#include "../../src/monte_carlo_tree/uecda_monte_carlo_tree_node.hpp"
#include "../../src/snowl_playout_utils.hpp"
#include "../../src/snowl_playout_policy.hpp"
#include "../../src/supervisor_playout_policy.hpp"

constexpr int kPlayoutSize = 1000;

UECdaState readLog(std::ifstream& fstream);

/* ソフトマックス関数の勾配。 */
SnowlPolicyVector gradientOfSoftMax(uecda::Cards my_cards, std::vector<uecda::Hand> hands, uecda::Hand hand, bool is_rev, SnowlPolicyVector weights);

#endif // LEARN_PLAYOUT_POLICY_HPP_