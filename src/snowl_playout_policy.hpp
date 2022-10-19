#ifndef SNOWL_ROLEOUT_POLICY_HPP_
#define SNOWL_ROLEOUT_POLICY_HPP_

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

#include "monte_carlo_tree/xorshift64.hpp"
#include "uecda_state.hpp"

/* ビットカード表現の強さを数値に変換する(3の強さを1とし、2の強さを13とする)。 */
int card_order2int(uecda::Cards::card_order card_order) {
  int result_reversed{}; // 14 - result。
  while (card_order != 0) {
    card_order >>= 1;
    result_reversed++;
  }
  return 15 - result_reversed;
}

/* snowlで`params`として定義されている、ロールアウトポリシー用のパラメータ。 */
/* 各手に対してスート無視の1対1対応で値が定義されている。 */
constexpr std::array<double, 166> snowl_params = {{
  /* 革命ではない場合。 */
  /* 5枚の階段(3始まり, 4始まり, ..., J始まり)。 */
  -0.165103445, -1.367353957, -1.084887944, -0.103446973,  0.005614329, -0.177613166,  0.020646977, -0.204253239,  0.018329075,
  /* 4枚の階段(3始まり, 4始まり, ..., Q始まり)。 */
  -1.250221599, -1.305045504, -1.456065252, -0.889654668, -2.987781810, -1.388516263, -1.176554458, -0.794348341, -2.055029278, -1.890634784,
  /* 3枚の階段(3始まり, 4始まり, ..., K始まり)。 */
  -3.704179332, -4.281589257, -4.719447787, -4.495022219, -5.089943875, -6.087235981, -2.976696921, -4.422503509, -4.221070745, -4.222361704, -4.002477132,
  /* 枚数組(1枚, 2枚, 3枚, 4枚)。 */
  -3.902118723, -5.356407798, -4.459366682, -1.673269738, // 3
  -3.813512676, -5.646004771, -4.959193566, -0.598674962, // 4
  -3.707419103, -5.827590172, -5.076551563, -0.455296345, // 5
  -2.914792373, -5.043195734, -4.436616845, -0.790830702, // 6
  -2.659857260, -4.498763022, -3.548758188, -0.164144807, // 7
  -6.162883027, -7.620724057, -6.552047173, -0.409127296, // 8
  -3.377120658, -4.432217918, -3.604395246, -0.447012953, // 9
  -2.931679825, -3.827014698, -2.405417588, -0.186615449, // 10
  -2.977701799, -3.088013233, -1.833889336, -0.034788011, // J
  -3.180260758, -2.933643112, -1.445919210,  0.025592376, // Q
  -3.039855900, -2.900483696, -1.851938754,  0.005744616, // K
  -2.396018281, -2.156369375, -1.014889482,  0.035431113, // A
  -2.386842708, -2.704110364, -2.010499011, -0.169877329, // 2
   0.557952043, // ジョーカー
  /* 革命の場合。 */
  /* 5枚の階段(3始まり, 4始まり, ..., J始まり)。 */
   0.015941512, -0.076186286, -1.869357120, -0.280747683, -0.003119470, -0.892415555,  0.001491174, -0.634278907, -1.064733776,
  /* 4枚の階段(3始まり, 4始まり, ..., Q始まり)。 */
  -1.496744051, -0.825525999, -2.733283880, -0.894995405, -1.898804633, -1.538144313, -1.025547067, -0.837688942, -1.185448678, -3.504837011,
  /* 3枚の階段(3始まり, 4始まり, ..., K始まり)。 */
  -4.213282878, -3.897294649, -4.012261980, -4.517719657, -5.015807231, -5.507484021, -4.051525440, -4.608462404, -4.057425508, -4.326998459, -5.129364509,
  /* 枚数組(1枚, 2枚, 3枚, 4枚)。 */
  -2.415693628, -2.782669751, -1.843946193, -0.004313174, // 3
  -2.704387616, -2.350158654, -1.209685749,  0.019062053, // 4
  -3.187466989, -2.919736669, -1.705203490, -0.060453580, // 5
  -3.442788490, -3.467241349, -2.027801109, -0.012216187, // 6
  -3.321312748, -3.495115522, -2.244767429, -0.129789376, // 7
  -5.744447193, -5.757793499, -5.355164182, -2.659961621, // 8
  -3.147674227, -3.078045473, -2.287018461, -0.368540465, // 9
  -2.965633813, -4.850912354, -3.522098268,  0.016110389, // 10
  -3.047409384, -4.952092305, -4.031908245, -0.387839210, // J
  -3.378235820, -5.678286406, -4.574724270, -0.436443491, // Q
  -3.720091654, -6.173770806, -5.468976055, -0.706931919, // K
  -4.073372779, -6.424423870, -5.079275011, -0.898643720, // A
  -4.051587944, -5.937719397, -5.274388701, -2.298538090, // 2
   0.848130302, // ジョーカー
}};

uecda::Hand snowlPlayoutPolicy(const UECdaState& state, XorShift64& random_engine) {
  const int my_playernum{state.getTable().whose_turn};
  const uecda::Table table{state.getTable()};
  const uecda::Cards my_cards{state.getPlayerCards().at(my_playernum)};
  const int my_cards_quantity{my_cards.quantity()};

  std::vector<uecda::Hand> legal_hands(state.legalActions());

  /* 出せる手が1つならそれを出す。 */
  if (legal_hands.size() == 1) { return legal_hands.at(0); }

  /* 各手の評価値を計算する。 */
  std::vector<double> evaluations(legal_hands.size(), 0);
  double evaluations_sum{};
  for (unsigned int i = 0; i < legal_hands.size(); i++) {
    const uecda::HandSummary summary{legal_hands.at(i).getSummary()};
    const bool can_revolute = (!summary.is_sequence && summary.quantity >= 4) || (summary.is_sequence && summary.quantity >= 5);

    /* 今見ている手で上がれるならそれを出す。 */
    if (summary.quantity == my_cards_quantity) { return legal_hands.at(i); }

    /* legal_handを出した後に構成できる手の集合。 */
    /* 集合においては、同じ手の型で重複するカードがないように最大の手のみを作る。これはほぼsnowlの仕様。 */
    const uecda::Cards rest_cards{my_cards - legal_hands.at(i).getWholeBitcards()};
    std::vector<uecda::Hand> rest_hands{};
    uecda::Hand::pushHandsWithoutOverlapInSameHandType(rest_cards, rest_hands);

    /* legal_handを出した後に構成できる手のそれぞれに対する評価値の総和をlegal_handの評価値とする。 */
    for (const uecda::Hand rest_hand : rest_hands) {
      const uecda::HandSummary rest_summary{rest_hand.getSummary()};
      int idx{}; // snowl_paramsの対応する添字。
      idx += (table.is_rev ^ can_revolute) ? 83 : 0; // 革命なら、革命用のパラメータの位置まで飛ばす。
      if (!rest_summary.is_sequence) {
        /* Hand::pushHandsWithoutOverlapInSameHandType()はジョーカーを1枚出しとしてのみ生成するので、
           ジョーカー1枚出しの場合とジョーカーなしの枚数組とで分ければよい。 */
        if (rest_summary.has_joker) {
          idx += 82;
        } else {
          idx += 30;
          idx += 4 * (card_order2int(rest_summary.weakest_order) - 1);
          idx += rest_summary.quantity - 1;
        }
      } else {
        /* 5枚より多い階段もすべて5枚の階段として扱う。 */
        if (rest_summary.quantity == 3) {
          idx += 19;
        } else if (rest_summary.quantity == 4) {
          idx += 9;
        }
        idx += card_order2int(rest_summary.weakest_order) - 1;
      }
      evaluations.at(i) += snowl_params.at(idx);
    }
    evaluations.at(i) = exp(evaluations.at(i));
    evaluations_sum += evaluations.at(i);
  }

  /* 重み付け抽選。ランダムなthretholdを重みの累積和が超えた時点での手を返す。 */
  std::uniform_real_distribution<double> dist(0, evaluations_sum);
  double threthold = dist(random_engine);
  double current_sum = 0.0;
  for (unsigned int i = 0; i < legal_hands.size(); i++) {
    current_sum += evaluations.at(i);
    if (threthold <= current_sum) { return legal_hands.at(i); }
  }

  std::cout << "threthold: " << threthold << std::endl;
  std::cout << "current_sum" << current_sum << std::endl;
  std::cout << "evaluations_sum" << evaluations_sum << std::endl;
  std::cerr << "制御が予期しない位置に達しました: snowl_playout_policy.hpp" << std::endl;
  exit(1);
}

#endif // SNOWL_ROLEOUT_POLICY_HPP_
