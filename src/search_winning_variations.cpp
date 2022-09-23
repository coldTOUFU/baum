#include "search_winning_variations.hpp"

bool pushTrump(const uecda::Cards& my_cards, const uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents, std::vector<uecda::Hand>& variation) {
  std::vector<uecda::Hand> hands{};
  uecda::Hand::pushHands(my_cards, hands);
  
  /* 出せば終わる手があれば、それを出して終わり。 */
  for (const uecda::Hand& h : hands) {
    if (!h.isLegal(table, table_hand)) { continue; }
    if (h.getSummary().quantity != my_cards.quantity()) { continue; }

    variation.insert(variation.begin(), h);
    return true;
  }

  /* 各切札について、それを出した後の必勝手順を探索していく。空の手札が返ってきたら、必勝手順。 */
  for (const uecda::Hand& h : hands) {
    if (!is_trump(h, table, record, table_hand, cards_of_opponents)) { continue; }

    /* hが革命を起こせる場合、次の探索に反映させるために状態を変える。 */
    uecda::Table next_table{table};
    next_table.is_rev ^= h.canRevolute();

    uecda::Cards next_my_cards{my_cards};
    next_my_cards -= h.getCards();
    if (h.getSummary().has_joker) { next_my_cards.deleteJoker(); }
    if (pushTrump(next_my_cards, next_table, record, {}, cards_of_opponents, variation)) {
      variation.insert(variation.begin(), h);
      return true;
    }
  } 

  return false;
}

std::vector<uecda::Hand> searchWinningVariation(const uecda::Cards& my_cards, uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents) {
  std::vector<uecda::Hand> result{};
  if (pushTrump(my_cards, table, record, table_hand, cards_of_opponents, result)) {
    return result;
  } else {
    return {};
  }
}
