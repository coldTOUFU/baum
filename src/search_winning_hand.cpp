#include "search_winning_hand.hpp"

bool anyOpponentsHaveNCards(const int n, const uecda::Table& table, const GameRecord& record) {
  for (unsigned int i = 0; i < table.card_quantity_of_players.size(); i++) {
    if (i != (unsigned int)table.whose_turn && !record.has_passed.at(i) && table.card_quantity_of_players.at(i) >= n) {
      return true;
    }
  }
  return false;
}

bool isTrump(const uecda::Hand& hand, uecda::Table table, const GameRecord& record, const uecda::Hand& table_hand, uecda::Cards cards_of_opponents) {
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

    /* しばり発生時は、相手の持ち札にフィルタをかける。 */
    if (table.is_lock || my_summary.suits == table_summary.suits) {
      uecda::Cards::bitcards suit_filter{};
      if (my_summary.suits % 2 == 1) {
        suit_filter = uecda::Cards::kCloverCards;
      } else if ((my_summary.suits >> 1) % 2 == 1) {
        suit_filter = uecda::Cards::kDiamondCards;
      } else if ((my_summary.suits >> 2) % 2 == 1) {
        suit_filter = uecda::Cards::kHeartCards;
      } else {
        suit_filter = uecda::Cards::kSpadeCards;
      }
      cards_of_opponents = cards_of_opponents.filterCards(suit_filter);
      /* 相手に出せるカードがない。 */
      if (cards_of_opponents.quantity() == 0) { return true; }
    }

    /* 相手の最強のカード以上の強さなら、またそのときに限り、場を流せる。 */
    const uecda::Cards strongest_card_of_opponent{!table.is_rev ? cards_of_opponents.strongestOrder() : cards_of_opponents.weakestOrder()};
    return !uecda::Hand::isFormerWeaker(table.is_rev, hand.getWholeBitcards(), strongest_card_of_opponent);
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
    const uecda::Cards strongest_card_of_opponent{!table.is_rev ? cards_of_opponents.strongestOrder() : cards_of_opponents.weakestOrder()};
    if (!uecda::Hand::isFormerWeaker(table.is_rev, hand.getWholeBitcards(), strongest_card_of_opponent)) { return true; }
  } else { /* 階段の場合。 */
    /* 枚数分出せるプレイヤがいなければ、場を流せる。 */
    if (!anyOpponentsHaveNCards(my_summary.quantity, table, record)) { return true; }

    int n{my_summary.quantity};

    /* 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。 */
    if (cards_of_opponents.hasJoker()) { n -= 1; }

    /* 相手の最強のカードとhandの最強のカードの強さがn未満しか離れていなければ、場を流せる。 */
    n -= (cards_of_opponents.hasJoker() ? 1 : 0); // 相手がジョーカーを持っていそうなら、ジョーカーを端に置けるので1枚分制約を緩くする。
    const uecda::Cards::bitcards lower_bound{!table.is_rev ? my_summary.strongest_order >> n : my_summary.weakest_order << n}; // 相手の最強のカードの下界。
    if (uecda::Hand::isFormerStronger(table.is_rev, uecda::Cards(lower_bound), cards_of_opponents)) { return true; }
  }

  /* 相手のカードで合法手が構成できなければ場を流せる。できるなら当然流せるとは言えない。 */
  std::vector<uecda::Hand> opponent_hands{};
  uecda::Hand::pushLegalHands(cards_of_opponents, opponent_hands, table, hand);
  for (const uecda::Hand& h : opponent_hands) {
    if (h.isLegal(table, hand)) { return false; }
  }
  return true;
}

// メモ: 最大深さは18くらい。
uecda::Hand searchWinningHand(const uecda::Cards& my_cards, const uecda::Table& table, const GameRecord& record, const uecda::Hand& table_hand, const uecda::Cards& cards_of_opponents) {
  std::vector<uecda::Hand> hands{};
  uecda::Hand::pushLegalHands(my_cards, hands, table, table_hand);
  
  /* 出せば終わる手があれば、それを出して終わり。 */
  for (const uecda::Hand& h : hands) {
    if (!h.isLegal(table, table_hand)) { continue; }
    if (h.getSummary().quantity != my_cards.quantity()) { continue; }

    return h;
  }

  // TODO: カードを2回以上出す場合、相手の最強カード以上のカードがなければ無理。階段とかも考慮の必要がある 

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
