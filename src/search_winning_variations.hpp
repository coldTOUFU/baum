#ifndef SEARCH_WINNING_VARIATIONS_HPP_
#define SEARCH_WINNING_VARIATIONS_HPP_

#include <vector>

#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "game_record.hpp"

bool someoneHasNCards(const int n, const uecda::Table& table, const GameRecord& record) {
  bool result{};
  for (unsigned int i = 0; i < table.card_quantity_of_players.size(); i++) {
    if (!record.has_passed.at(i) && table.card_quantity_of_players.at(i) >= n) {
      result = true;
      break;
    }
  }
  return result;
}

/* trump(切札: 確実に場を流せる)か？ */
bool is_trump(const uecda::Hand& hand, uecda::Table table, const GameRecord record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents) {
  if (!hand.isLegal(table, table_hand)) { return false; }

  /* 8切は問答無用で場を流す。 */
  constexpr uecda::Cards::bitcards eights{(uecda::Cards::bitcards)9007474141036800};
  if (hand.getWholeBitcards().filterCards(eights) != (uecda::Cards::bitcards)0) { return true; }

  /* スぺ3返しは問答無用で場を流す。 */
  constexpr uecda::Cards::bitcards spade3{(uecda::Cards::bitcards)0b010000000000000 << 45};
  if (table_hand.getSummary().has_joker && table_hand.getSummary().quantity == 1 && hand.getWholeBitcards() == spade3) { return true; }

  /* 1枚出しの場合。 */
  if (hand.getSummary().quantity == 1) {
    /* 相手がジョーカーを持っていたら場を流せない。 */
    if (cards_of_opponents.hasJoker()) { return false; }

    /* 相手の最強のカード以上の強さなら、またそのときに限り、場を流せる。 */
    return ((!table.is_rev && hand.getSummary().strongest_order <= cards_of_opponents.strongestOrder()) ||
        (table.is_rev && hand.getSummary().weakest_order >= cards_of_opponents.weakestOrder()));
  }

  /* 以下複数枚の場合なので、handが革命を起こせるなら反映させる。 */
  table.is_rev ^= ((!hand.getSummary().is_sequence && hand.getSummary().quantity >= 4) || (hand.getSummary().is_sequence && hand.getSummary().quantity >= 5));

  /* とりあえず、
  /* n(> 1)枚出しの場合。 */
  if (!hand.getSummary().is_sequence) {
    /* 枚数分出せるプレイヤがいなければ、場を流せる。ここは、各々のプレイヤの手札枚数に着目しており、最後の合法手の存在判定とは別に必要。 */
    if (!someoneHasNCards(hand.getSummary().quantity, table, record)) { return true; }

    /* 相手の最強のカード以上の強さなら、場を流せる。なくてもいいけど、相手の手札群から手を生成して合法判定するのはコストがかかるのでここで引っ掛けて節約したいなという気持ち。 */
    if ((!table.is_rev && hand.getSummary().strongest_order <= cards_of_opponents.strongestOrder()) ||
        (table.is_rev && hand.getSummary().weakest_order >= cards_of_opponents.weakestOrder())) { return true; }
  } else { /* 階段の場合。 */
    /* 枚数分出せるプレイヤがいなければ、場を流せる。 */
    if (!someoneHasNCards(hand.getSummary().quantity, table, record)) { return true; }

    int n{hand.getSummary().quantity};

    /* handに対して出せるカードが存在しない場合は、場を流せる。ここをパスした場合、相手がジョーカーを端に置いて階段が出せることを保証できるので、直下で何も考えずnから1を引ける。 */
    if ((!table.is_rev && hand.getSummary().strongest_order < (1 << (n - 1))) ||
        (table.is_rev && hand.getSummary().weakest_order > (1 >> (n - 1)))) { return true; }

    /* 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。 */
    if (cards_of_opponents.hasJoker()) { n -= 1; }

    /* 相手の最強のカードとhandの最強のカードの強さがn未満しか離れていなければ、場を流せる。 */
    const int n{hand.getSummary().quantity - (cards_of_opponents.hasJoker() ? 1 : 0)}; // 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。
    if ((!table.is_rev && (hand.getSummary().strongest_order >> n) < cards_of_opponents.strongestOrder()) ||
        (table.is_rev && (hand.getSummary().weakest_order << n) > cards_of_opponents.weakestOrder())) { return true; }
  }

  /* 相手のカードで合法手が構成できなければ場を流せる。できるなら当然流せるとは言えない。 */
  std::vector<uecda::Hand> opponent_hands{};
  uecda::Hand::pushHands(cards_of_opponents, opponent_hands);
  for (const uecda::Hand& h : opponent_hands) {
    if (h.isLegal(table, hand)) { return false; }
  }
  return true;
}

#endif // SEARCH_WINNING_VARIATIONS_HPP_
