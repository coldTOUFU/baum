#ifndef SNOWL_SUPERVISOR_PLAYOUT_POLICY_HPP_
#define SNOWL_SUPERVISOR_PLAYOUT_POLICY_HPP_

#include "uecda_cpp/hand.hpp"
#include "uecda_state.hpp"
#include "monte_carlo_tree/monte_carlo_tree_node.hpp"
#include "monte_carlo_tree/uecda_monte_carlo_tree_node.hpp"
#include "monte_carlo_tree/xorshift64.hpp"

/* snowlで使われる教師データ用のプレイアウトポリシー。 */
/* ランダムプレイアウトを用いたモンテカルロ木探索を行う。遅いので実戦では使えない。 */
uecda::Hand supervisorPlayoutPolicy(const UECdaState& state, XorShift64& random_engine) {
  MonteCarloTreeNode<UECdaState, uecda::Hand, 5> mctnode = MonteCarloTreeNode<UECdaState, uecda::Hand, 5>(state, state.getMyPlayerNum(), random_engine());
  return mctnode.search();
}

#endif // SNOWL_SUPERVISOR_PLAYOUT_POLICY_HPP_

