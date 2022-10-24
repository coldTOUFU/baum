#ifndef SNOWL_PLAYOUT_UTILS_HPP_
#define SNOWL_PLAYOUT_UTILS_HPP_

#include <array>

#include "uecda_cpp/hand.hpp"

/* snowlの方策関数で使われるベクトル。 */
using SnowlPolicyVector = std::array<double, 166>;

/* ビットカード表現の強さを数値に変換する(3の強さを1とし、2の強さを13とする)。 */
int card_order2int(uecda::Cards::card_order card_order) {
  int result_reversed{}; // 14 - result。
  while (card_order != 0) {
    card_order >>= 1;
    result_reversed++;
  }
  return 15 - result_reversed;
}

/* handに対応するsnowl方策ベクトルのインデックスを返す。 */
int hand2SnowlPolicyVectorIdx(const uecda::Hand& hand, const bool is_rev) {
  const uecda::HandSummary summary{hand.getSummary()};
  int idx{}; // ベクトルの対応する添字。
  idx += is_rev ? 83 : 0; // 革命なら、革命用のパラメータの位置まで飛ばす。
  if (!summary.is_sequence) {
    /* Hand::pushHandsWithoutOverlapInSameHandType()はジョーカーを1枚出しとしてのみ生成するので、
       ジョーカー1枚出しの場合とジョーカーなしの枚数組とで分ければよい。 */
    if (summary.has_joker) {
      idx += 82;
    } else {
      idx += 30;
      idx += 4 * (card_order2int(summary.weakest_order) - 1);
      idx += summary.quantity - 1;
    }
  } else {
    /* 5枚より多い階段もすべて5枚の階段として扱う。 */
    if (summary.quantity == 3) {
      idx += 19;
    } else if (summary.quantity == 4) {
      idx += 9;
    }
    idx += card_order2int(summary.weakest_order) - 1;
  }
  return idx;
}

/* 与えられた重みベクトルに基づき、snowl同様の仕方でhandの評価値を計算して返す。 */
int snowlEvaluation(const uecda::Cards& my_cards, const uecda::Hand& hand, const bool is_rev, const SnowlPolicyVector& weights) {
  const uecda::HandSummary summary{hand.getSummary()};
  const bool can_revolute = (!summary.is_sequence && summary.quantity >= 4) || (summary.is_sequence && summary.quantity >= 5);

  double evaluation{};

  /* 手を出した後に構成できる手の集合。 */
  /* 集合においては、同じ手の型で重複するカードがないように最大の手のみを作る。これはsnowlの仕様。 */
  const uecda::Cards rest_cards{my_cards - hand.getWholeBitcards()};
  std::vector<uecda::Hand> rest_hands{};
  uecda::Hand::pushHandsWithoutOverlapInSameHandType(rest_cards, rest_hands);

  /* 手を出した後に構成できる手のそれぞれに対する評価値の総和を、その手の評価値とする。 */
  for (const uecda::Hand rest_hand : rest_hands) {
    const int idx{hand2SnowlPolicyVectorIdx(rest_hand, (is_rev ^ can_revolute))};
    evaluation += weights.at(idx);
  }

  return evaluation;
}

/* 手に対応する特徴量ベクトルを返す。 */
SnowlPolicyVector snowlFeatureVector(const uecda::Cards& my_cards, const uecda::Hand& hand, const bool is_rev) {
  const uecda::HandSummary summary{hand.getSummary()};
  const bool can_revolute = (!summary.is_sequence && summary.quantity >= 4) || (summary.is_sequence && summary.quantity >= 5);

  SnowlPolicyVector result{};

  /* 手を出した後に構成できる手の集合。 */
  /* 集合においては、同じ手の型で重複するカードがないように最大の手のみを作る。これはsnowlの仕様。 */
  const uecda::Cards rest_cards{my_cards - hand.getWholeBitcards()};
  std::vector<uecda::Hand> rest_hands{};
  uecda::Hand::pushHandsWithoutOverlapInSameHandType(rest_cards, rest_hands);

  /* 手を出した後に構成できる手のそれぞれに対する評価値の総和を、その手の評価値とする。 */
  for (const uecda::Hand rest_hand : rest_hands) {
    const int idx{hand2SnowlPolicyVectorIdx(rest_hand, (is_rev ^ can_revolute))};
    result.at(idx)++;
  }

  return result;
}

#endif // SNOWL_PLAYOUT_UTILS_HPP_
