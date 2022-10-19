#ifndef DEFAULT_PLAYOUT_POLICY_HPP_
#define DEFAULT_PLAYOUT_POLICY_HPP_

#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/sample_client/select_hand.hpp"
#include "uecda_state.hpp"
#include "monte_carlo_tree/xorshift64.hpp"

uecda::Hand defaultPlayoutPolicy(const UECdaState& state, XorShift64& random_engine) {
  std::vector<uecda::Hand> legal_hands = state.legalActions();
  return select_hand(legal_hands, state.getTableHand(), state.getTable());
}

#endif // DEFAULT_PLAYOUT_POLICY_HPP_
