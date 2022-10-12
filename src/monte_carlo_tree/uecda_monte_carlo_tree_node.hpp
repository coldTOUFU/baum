#ifndef UECDA_MONTE_CARLO_TREE_NODE_HPP_
#define UECDA_MONTE_CARLO_TREE_NODE_HPP_

#include "monte_carlo_tree_node.hpp"
#include "../search_winning_hand.hpp"
#include "../uecda_cpp/cards.hpp"
#include "../uecda_cpp/hand.hpp"
#include "../uecda_cpp/table.hpp"
#include "../uecda_state.hpp"

template<>
const uecda::Hand MonteCarloTreeNode<UECdaState, uecda::Hand, 5>::epsilonGreedyAction(UECdaState& first_state) {
  std::default_random_engine rand_engine(random_seed_);
  std::uniform_real_distribution<float> dist(0.0, 1.0);

  const uecda::Table table{first_state.getTable()};
  const GameRecord record{first_state.getRecord()};
  uecda::Cards cards_of_opponents{};
  for (int i = 0; i < 5; i++) {
    if (i == table.whose_turn) { continue; }
    cards_of_opponents += first_state.getPlayerCards().at(i);
  }

  /* 自分がパスを出せば上がる状況のとき、パスを出す。 */
  if (record.last_submitted_player == table.whose_turn &&
      std::count_if(record.has_passed.begin(), record.has_passed.end(), [](bool b) { return b; }) == 4) {
    return {};
  }

  /* 必勝手探索。 */
  uecda::Hand submission_hand{searchWinningHand(first_state.getPlayerCards().at(table.whose_turn), table, record, first_state.getTableHand(), cards_of_opponents)};
  if (!submission_hand.getSummary().is_pass) { return submission_hand; }

  /* ロールアウトポリシーを使う。 */
  if (dist(rand_engine) <= epsilon_) {
    return randomAction(first_state);
  } else {
    return selectForPlayout_(first_state);
  }
}

#endif // UECDA_MONTE_CARLO_TREE_NODE_HPP_
