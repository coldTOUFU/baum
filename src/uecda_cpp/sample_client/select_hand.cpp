#include "select_hand.hpp"

std::vector<Hand> select_change_hands(const std::vector<Hand> &hands) {
  std::vector<Hand> single_without_joker;

  /* 交換候補として、ジョーカー以外の1枚出しを取り出す */
  for (const Hand& hand : hands) {
    const HandSummary summary{hand.getSummary()};
    if (!summary.has_joker && summary.quantity == 1) {
      single_without_joker.push_back(hand);
    }
  }

  /* 強さの昇順に整列。交換枚数がnの場合、0からn個とればいいようにする。 */
  sort(single_without_joker.begin(), single_without_joker.end(), [](Hand &h1, Hand &h2) {
    return h1.getSummary().strongest_order > h2.getSummary().strongest_order;
  });

  return single_without_joker;
}

/* 同じ型の手の中から、手の枚数と強さの観点のみから最善の手を選ぶ。handsには1つ以上の合法手が存在すると仮定。 */
Hand select_best_hand_in_same_hand_type(const std::vector<Hand>& hands, bool is_rev) {
  const Hand* best_hand{&hands.at(0)};
  for (const Hand& hand : hands) {
    const HandSummary& best_summary{best_hand->getSummary()};
    const HandSummary& cur_summary{hand.getSummary()};
    if (cur_summary.quantity < best_summary.quantity) {
      continue;
    }
    else if (cur_summary.quantity > best_summary.quantity) {
      best_hand = &hand;
    }
    else if (!is_rev && cur_summary.strongest_order > best_summary.strongest_order) {
      best_hand = &hand;
    }
    else if (is_rev && cur_summary.strongest_order < best_summary.strongest_order) {
      best_hand = &hand;
    }
  }
  return *best_hand;
}

Hand select_hand(std::vector<Hand>& hands, const Hand& table_hand, const Table& table) {
  /* 手の候補がなければ(あり得ないが)、パスの手を返す。 */
  if (hands.size() <= 0) {
    return Hand();
  }

  std::vector<Hand> single_without_joker{};
  std::vector<Hand> pair_without_joker{};
  std::vector<Hand> sequence_without_joker{};
  std::vector<Hand> single_with_joker{};
  std::vector<Hand> pair_with_joker{};
  std::vector<Hand> sequence_with_joker{};

  /* 枚数が多い手に含まれるカードを考慮から除外することで、枚数の多い手を崩さないようにする。 */
  Cards pair_excluded_cards{};
  Cards sequence_excluded_cards{};

  /* 枚数の多い手を先に確認して*_excluded_cardsに加えるために、枚数の多い順に並び替える。 */
  std::sort(hands.begin(), hands.end(), [](Hand a, Hand b) {
    return a.getSummary().quantity > b.getSummary().quantity;
  });

  /* 着手候補を手の種類・ジョーカーの有無で別々の配列に分ける。 */
  for (const Hand &hand : hands) {
    const HandSummary summary{hand.getSummary()};

    /* 出せる手の枚数が確定しているなら、より枚数の多い手に含まれるカードを出さないようにする。 */
    if (!table.is_start_of_trick && !summary.has_joker && summary.quantity > table_hand.getSummary().quantity) {
      if (summary.is_sequence) {
        sequence_excluded_cards += hand.getCards();
      } else {
        pair_excluded_cards += hand.getCards();
      }
    }

    /* 非合法なら出さない。 */
    if (!hand.isLegal(table, table_hand)) {
      continue;
    }

    /* *_excluded_cardsに含まれるカードがあるなら、手を出さない。 */
    /* ただし、(オリジナルのdefaultでそうしているので、)縛り中は気にしない。 */
    if (!table.is_lock && summary.is_sequence && sequence_excluded_cards.hasAnyOf(hand.getCards())) { continue; }
    else if (!table.is_lock && pair_excluded_cards.hasAnyOf(hand.getCards())) { continue; }
    /* 1枚出しの場合は、縛りがあってもペアや階段を崩さないようにする。 */
    if (summary.quantity == 1 &&
        (sequence_excluded_cards.hasAnyOf(hand.getCards()) || pair_excluded_cards.hasAnyOf(hand.getCards()))) {
      continue;
    }

    /* 手の性質に合わせてそれぞれの配列に挿入する。 */
    if (!summary.has_joker) {
      if (summary.quantity == 1) {
        single_without_joker.push_back(hand);
      } else if (!summary.is_sequence) {
        pair_without_joker.push_back(hand);
      } else {
        sequence_without_joker.push_back(hand);
      }
    } else {
      if (summary.quantity == 1) {
        single_with_joker.push_back(hand);
      } else if (!summary.is_sequence) {
        pair_with_joker.push_back(hand);
      } else {
        sequence_with_joker.push_back(hand);
      }
    }
  }

  /* 場にカードがある場合: 階段→n(>1)枚出し→1枚出し、ジョーカーなし→ジョーカーありの順で手を探索 */
  /* 場にカードがない場合: 階段→n(>1)枚出し→1枚出しの順で手を探索(ジョーカー区別なし) */
  if (!table.is_start_of_trick) {
    if (sequence_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(sequence_without_joker, table.is_rev);
    }

    if (sequence_with_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(sequence_with_joker, table.is_rev);
    }

    if (pair_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(pair_without_joker, table.is_rev);
    }

    if (pair_with_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(pair_with_joker, table.is_rev);
    }

    if (single_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(single_without_joker, table.is_rev);
    }

    if (single_with_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(single_with_joker, table.is_rev);
    }
  } else {
    sequence_without_joker.insert(sequence_without_joker.end(), sequence_with_joker.begin(), sequence_with_joker.end());
    pair_without_joker.insert(pair_without_joker.end(), pair_with_joker.begin(), pair_with_joker.end());
    single_without_joker.insert(single_without_joker.end(), single_with_joker.begin(), single_with_joker.end());
    if (sequence_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(sequence_without_joker, table.is_rev);
    }
    if (pair_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(pair_without_joker, table.is_rev);
    }
    if (single_without_joker.size() > 0) {
      return select_best_hand_in_same_hand_type(single_without_joker, table.is_rev);
    }
  }

  /* 合法手がなかった場合はパスを返す。 */
  return Hand();
}
