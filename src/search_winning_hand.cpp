#include "search_winning_hand.hpp"

bool anyOpponentsHaveNCards(const int n, const uecda::Table& table, const GameRecord& record) {
  for (unsigned int i = 0; i < table.card_quantity_of_players.size(); i++) {
    if (i != (unsigned int)table.whose_turn && !record.has_passed.at(i) && table.card_quantity_of_players.at(i) >= n) {
      return true;
    }
  }
  return false;
}

bool isTrump(const uecda::Hand& hand, uecda::Table table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents) {
  if (!hand.isLegal(table, table_hand)) { return false; }

  const uecda::HandSummary my_summary{hand.getSummary()};
  const uecda::HandSummary table_summary{table_hand.getSummary()};

  /* 8切は問答無用で場を流す。 */
  constexpr uecda::Cards::bitcards eights{uecda::Cards::S8 | uecda::Cards::H8 | uecda::Cards::D8 | uecda::Cards::C8};
  if (hand.getWholeBitcards().filterCards(eights) != (uecda::Cards::bitcards)0) { return true; }

  /* スぺ3返しは問答無用で場を流す。 */
  if (table_summary.has_joker && table_summary.quantity == 1 && hand.getWholeBitcards() == uecda::Cards::S3) { return true; }

  /* 1枚出しの場合。 */
  if (my_summary.quantity == 1) {
    /* 相手がジョーカーを持っていたら場を流せない。 */
    if (cards_of_opponents.hasJoker()) { return false; }

    /* 相手の最強のカード以上の強さなら、またそのときに限り、場を流せる。1枚出しなので、革命時にweakest_orderで比較する必要はない。 */
    return !uecda::Hand::isFormerWeaker(table.is_rev, my_summary.strongest_order, cards_of_opponents.strongestOrder());
  }

  /* 以下でhandを出した後の合法手について考えるので、tableを更新する。 */
  table.is_rev ^= ((!my_summary.is_sequence && my_summary.quantity >= 4) || (my_summary.is_sequence && my_summary.quantity >= 5));
  table.is_start_of_trick = false;
  table.is_lock = hand.getSummary().suits == table_hand.getSummary().suits;

  /* n(> 1)枚出しの場合。 */
  if (!my_summary.is_sequence) {
    /* 枚数分出せるプレイヤがいなければ、場を流せる。ここは、各々のプレイヤの手札枚数に着目しており、最後の合法手の存在判定とは別に必要。 */
    if (!anyOpponentsHaveNCards(my_summary.quantity, table, record)) { return true; }

    /* 相手の最強のカード以上の強さなら、場を流せる。なくてもいいけど、相手の手札群から手を生成して合法判定するのはコストがかかるのでここで引っ掛けて時間節約したいなという気持ち。 */
    if ((!table.is_rev && !uecda::Hand::isFormerWeaker(table.is_rev, my_summary.strongest_order, cards_of_opponents.strongestOrder())) ||
        (table.is_rev && !uecda::Hand::isFormerWeaker(table.is_rev, my_summary.weakest_order, cards_of_opponents.weakestOrder()))) { return true; }
  } else { /* 階段の場合。 */
    /* 枚数分出せるプレイヤがいなければ、場を流せる。 */
    if (!anyOpponentsHaveNCards(my_summary.quantity, table, record)) { return true; }

    int n{my_summary.quantity};

    /* 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。 */
    if (cards_of_opponents.hasJoker()) { n -= 1; }

    /* 相手の最強のカードとhandの最強のカードの強さがn未満しか離れていなければ、場を流せる。 */
    n -= (cards_of_opponents.hasJoker() ? 1 : 0); // 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。
    const uecda::Cards::bitcards lower_bound{!table.is_rev ? my_summary.strongest_order >> n : my_summary.weakest_order << n}; // 相手の最強のカードの下界。
    if ((!table.is_rev && uecda::Hand::isFormerStronger(table.is_rev, lower_bound, cards_of_opponents.strongestOrder())) ||
        (table.is_rev && uecda::Hand::isFormerStronger(table.is_rev, lower_bound, cards_of_opponents.weakestOrder()))) { return true;}
  }

  /* 相手のカードで合法手が構成できなければ場を流せる。できるなら当然流せるとは言えない。 */
  std::vector<uecda::Hand> opponent_hands{};
  uecda::Hand::pushHands(cards_of_opponents, opponent_hands);
  for (const uecda::Hand& h : opponent_hands) {
    if (h.isLegal(table, hand)) { return false; }
  }
  return true;
}

uecda::Hand searchWinningHand(const uecda::Cards& my_cards, const uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents) {
  std::vector<uecda::Hand> hands{};
  uecda::Hand::pushHands(my_cards, hands);
  
  /* 出せば終わる手があれば、それを出して終わり。 */
  for (const uecda::Hand& h : hands) {
    if (!h.isLegal(table, table_hand)) { continue; }
    if (h.getSummary().quantity != my_cards.quantity()) { continue; }

    return h;
  }

  /* 各切札について、それを出した後の必勝手順を探索する。必勝手順であれば空ではない手が返ってくるので、そのときの切札を返せばよい。 */
  for (const uecda::Hand& h : hands) {
    if (!isTrump(h, table, record, table_hand, cards_of_opponents)) { continue; }

    /* hが革命を起こせる場合、次の探索に反映させるために状態を変える。 */
    uecda::Table next_table{table};
    next_table.is_rev ^= h.canRevolute();

    uecda::Cards next_my_cards{my_cards};
    next_my_cards -= h.getCards();
    if (h.getSummary().has_joker) { next_my_cards.deleteJoker(); }
    if (!searchWinningHand(next_my_cards, next_table, record, {}, cards_of_opponents).getSummary().is_pass) { return h; }
  } 

  return {};
}
