#include "hand.hpp"

bool uecda::Hand::isLegal(const Table &tbl, const Hand &table_hand) const {
  if (tbl.is_start_of_trick) { return true; }
  if (summary_.is_pass) { return true; }

  const HandSummary table_hand_summary{table_hand.getSummary()};

  /* ジョーカー1枚出しは最強。 */
  if (summary_.quantity == 1 && table_hand_summary.quantity == 1 && summary_.has_joker) { return true; }

  /* 相手がジョーカー1枚出しなら、スぺ3返し以外はできない。 */
  if (table_hand_summary.quantity == 1 && table_hand_summary.has_joker) {
    return summary_.quantity == 1 && cards_.filterCards(Cards::S3) != (Cards::bitcards)0;
  }

  /* 場と同じ種類の手である必要がある。 */
  if (summary_.is_sequence != table_hand_summary.is_sequence)  { return false; }

  /* 場のカードと同枚数である必要がある。 */
  if (summary_.quantity != table_hand_summary.quantity) { return false; }

  /* 出すカードが場のカードより強い必要がある。 */
  if (!isFormerStronger(tbl.is_rev, this->getWholeBitcards(), table_hand.getWholeBitcards())) { return false; }

  /* しばりなら、スートが一致する必要がある。 */
  if (tbl.is_lock && summary_.suits != table_hand_summary.suits) { return false; }

  return true;
}

void uecda::Hand::pushHands(const Cards &src, std::vector<Hand>& hand_vec) {
  Cards::bitcards src_bit{src.toBitcards()};
  /* 手探索でフィルターにかけるとき邪魔なので、ジョーカーのビットを落とす。  */
  src_bit &= (Cards::bitcards)0xfffffffffffffff;

  for (int pair_qty = 1; pair_qty <= 4; pair_qty++) {
    Hand::pushPair(src_bit, hand_vec, pair_qty);
  }
  for (int seq_qty = 3; seq_qty <= 14; seq_qty++) {
    Hand::pushSequence(src_bit, hand_vec, seq_qty);
  }

  /* ジョーカーがあればジョーカーを用いた役を追加。 */
  if (src.hasJoker()) {
    /* ジョーカー単騎 */
    const Cards::bitcards c{};
    const Cards::bitcards j{(Cards::bitcards)1};
    hand_vec.push_back(Hand::bitcards2Hand(c, j));

    for (int pair_qty = 2; pair_qty <= 4; pair_qty++) {
      Hand::pushPairWithJoker(src_bit, hand_vec, pair_qty);
    }
    for (int seq_qty = 3; seq_qty <= 14; seq_qty++) {
      Hand::pushSequenceWithJoker(src_bit, hand_vec, seq_qty);
    }
  }
}

void uecda::Hand::pushLegalHands(const Cards& src, std::vector<Hand>& hand_vec, const Table& table, const Hand& table_hand) {
  Cards::bitcards src_bit{src.toBitcards()};
  /* 手探索でフィルターにかけるとき邪魔なので、ジョーカーのビットを落とす。  */
  src_bit &= (Cards::bitcards)0xfffffffffffffff;

  /* 場が空なら、何を出してもいいので作れる手を全部突っ込む。 */
  if (table.is_start_of_trick) {
    Hand::pushHands(src, hand_vec);
    return;
  }

  std::vector<Hand> tmp_hand_vec{}; // 後で合法手だけをhand_vecに入れるための、一時的な配列。
  if (!table_hand.getSummary().is_sequence) {
    Hand::pushPair(src_bit, tmp_hand_vec, table_hand.getSummary().quantity);
    if (src.hasJoker()) {
      /* 場が1枚出しの場合は、ジョーカー1枚出しが何通りも生成されないようにする。 */
      if (table_hand.getSummary().quantity == 1) {
        const Cards::bitcards c{};
        const Cards::bitcards j{(Cards::bitcards)1};
        tmp_hand_vec.push_back(Hand::bitcards2Hand(c, j));
      } else {
        Hand::pushPairWithJoker(src_bit, tmp_hand_vec, table_hand.getSummary().quantity);
      }
    }
  } else {
    Hand::pushSequence(src_bit, tmp_hand_vec, table_hand.getSummary().quantity);
    if (src.hasJoker()) {
      Hand::pushSequenceWithJoker(src_bit, tmp_hand_vec, table_hand.getSummary().quantity);
    }
  }

  for (const Hand& h : tmp_hand_vec) {
    if (h.isLegal(table, table_hand)) { hand_vec.push_back(h); }
  }
}

void uecda::Hand::pushHandsWithoutOverlapInSameHandType(const Cards& src, std::vector<Hand>& hand_vec) {
  Cards::bitcards src_bit_for_pair{src.toBitcards()};
  Cards::bitcards src_bit_for_sequence{src.toBitcards()};
  /* 手探索でフィルターにかけるとき邪魔なので、ジョーカーのビットを落とす。  */
  src_bit_for_pair &= (Cards::bitcards)0xfffffffffffffff;
  src_bit_for_sequence &= (Cards::bitcards)0xfffffffffffffff;

  for (int pair_qty = 4; pair_qty >= 1; pair_qty--) {
    Hand::pushPairAndTakeAwayFromSrc(src_bit_for_pair, hand_vec, pair_qty);
  }
  for (int seq_qty = 14; seq_qty >= 3; seq_qty--) {
    Hand::pushSequenceAndTakeAwayFromSrc(src_bit_for_sequence, hand_vec, seq_qty);
  }

  /* ジョーカーがあればジョーカー1枚出しを追加。 */
  if (src.hasJoker()) {
    const Cards::bitcards c{};
    const Cards::bitcards j{(Cards::bitcards)1};
    hand_vec.push_back(Hand::bitcards2Hand(c, j));
  }
}

void uecda::Hand::putCards(uecda::common::CommunicationBody& dst) const {
  Cards::bitcards src{cards_.toBitcards()};
  Cards::bitcards src_joker{joker_.toBitcards()};

  for (int i = 3; i >= 0; i--) {
    for (int j = 14; j >= 0; j--) {
      dst.at(i).at(j) = src % 2;
      /* ジョーカーがある場合 */
      if (src_joker % 2) {
        dst.at(i).at(j) = 2;
      }

      src >>= 1;
      src_joker >>= 1;
    }
  }
}

uecda::Cards uecda::Hand::createCards(const uecda::common::CommunicationBody& src) {
  Cards::bitcards b{};

  for (int suit = 0; suit < 4; suit++) {
    for (int order = 0; order < 15; order++) {
      if (src.at(suit).at(order) == 1) {
        b++;
      }
      b <<= 1;
    }
  }

  /* 上で余計に1回左シフトした分を戻す。 */
  b >>= 1;
  return Cards(b);
}

uecda::Cards uecda::Hand::createJoker(const uecda::common::CommunicationBody& src) {
  Cards::bitcards b{};

  for (int suit = 0; suit < 4; suit++) {
    for (int order = 0; order < 15; order++) {
      if (src.at(suit).at(order) == 2) {
        b++;
      }
      b <<= 1;
    }
  }

  /* 上で余計に1回左シフトした分を戻す。 */
  b >>= 1;
  return Cards(b);
}

uecda::HandSummary uecda::Hand::summarize(const Cards::bitcards src, const Cards::bitcards joker_src) {
  const Cards src_card{Cards(src)};
  const Cards joker_src_card{Cards(joker_src)};

  /* パスのサマリ。 */
  if (src_card.quantity() == 0 && joker_src_card.quantity() == 0) {
    return {};
  }

  const Cards::card_order w_ord{std::max(src_card.weakestOrder(), joker_src_card.weakestOrder())};
  /* s_ordについては、カードが空の場合見かけ上その強さが0(最強)になってしまうので、カードが空かどうかで場合分けする。 */
  Cards::bitcards s_ord;
  if (src_card.toBitcards() <= 0) {
    s_ord = joker_src_card.strongestOrder();
  } else if (joker_src_card.toBitcards() <= 0) {
    s_ord = src_card.strongestOrder();
  } else {
    s_ord = std::min(src_card.strongestOrder(), joker_src_card.strongestOrder());
  }

  const int suits{(src_card.getSuits() | joker_src_card.getSuits())};

  bool is_sequence;
  if (w_ord == s_ord) {
    is_sequence = false;
  } else if (suits == 0b0001 || suits == 0b0010 || suits == 0b0100 || suits == 0b1000) {
    is_sequence = true;
  } else {
    throw CannotConvertToHandException();
  }

  return {
    src_card.quantity() + joker_src_card.quantity(),
    false,
    is_sequence,
    w_ord,
    s_ord,
    joker_src_card.quantity() > 0,
    src_card.getSuits() | joker_src_card.getSuits()
  };
}

void uecda::Hand::pushPair(const Cards::bitcards src, std::vector<Hand> &hand_vec, const int pair_qty) {
  if (pair_qty < 1 || pair_qty > 4) { return; } // フィルターを定義していないseq_qtyが来たら何もしない。

  /* 各フィルター(=スートの組み合わせ)に対し、各数字についてペアを探す。 */
  for (int i = 0; i < Hand::kPairFilterSize.at(pair_qty - 1); i++) {
    for (int j = 0; j < 15; j++){
      const Cards::bitcards tmpfilter{Hand::kPairFilters.at(pair_qty - 1).at(i) << j};
      if ((src & tmpfilter) == tmpfilter) {
        const Cards::bitcards j{};
        hand_vec.push_back(Hand::bitcards2Hand(tmpfilter, j));
      }
    }
  }
}

void uecda::Hand::pushPairWithJoker(const Cards::bitcards src, std::vector<Hand> &hand_vec, const int pair_qty) {
  if (pair_qty < 1 || pair_qty > 4) { return; } // フィルターを定義していないseq_qtyが来たら何もしない。

  for (int i = 0; i < Hand::kPairFilterSize.at(pair_qty - 1); i++) {
    for (int j = 0; j < 15; j++) {
      const Cards::bitcards tmpfilter{Hand::kPairFilters.at(pair_qty - 1).at(i) << j};
      if (Cards::count(src & tmpfilter) == pair_qty - 1) {
        const Cards::bitcards c{src & tmpfilter};
        const Cards::bitcards j{~src & tmpfilter};
        hand_vec.push_back(Hand::bitcards2Hand(c, j));
      }
    }
  }
}

void uecda::Hand::pushSequence(const Cards::bitcards src, std::vector<Hand> &hand_vec, const int seq_qty) {
  if (seq_qty < 3 || seq_qty > 14) { return; } // フィルターを定義していないseq_qtyが来たら何もしない。

  /* 各スートに対し、各数字に対して階段を探す。 */
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j <= 15 - seq_qty; j++) {
      /* filterを、i番目のスートでj番目から始めるものに適用できるようシフトする。 */
      const Cards::bitcards tmpfilter{Hand::kSequenceFilters.at(seq_qty - 1) << (15 * i + j)};
      if((src & tmpfilter) == tmpfilter) { 
        const Cards::bitcards j{};
        hand_vec.push_back(Hand::bitcards2Hand(tmpfilter, j));
      }
    }
  }
}

void uecda::Hand::pushSequenceWithJoker(const Cards::bitcards src, std::vector<Hand> &hand_vec, const int seq_qty) {
  if (seq_qty < 3 || seq_qty > 14) { return; }

  /* 各スートに対し、各数字に対して階段を探す。 */
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j <= 15 - seq_qty; j++) {
      /* filterを、i番目のスートでj番目から始めるものに適用できるようシフトする。 */
      const Cards::bitcards tmpfilter{Hand::kSequenceFilters.at(seq_qty - 1) << (15 * i + j)};
      if (Cards::count(src & tmpfilter) == seq_qty - 1) {
        const Cards::bitcards c{src & tmpfilter};
        const Cards::bitcards j{~src & tmpfilter};
        hand_vec.push_back(Hand::bitcards2Hand(c, j));
      }
    }
  }
}

void uecda::Hand::pushPairAndTakeAwayFromSrc(Cards::bitcards& src, std::vector<Hand> &hand_vec, const int pair_qty) {
  if (pair_qty < 1 || pair_qty > 4) { return; } // フィルターを定義していないseq_qtyが来たら何もしない。

  /* 各フィルター(=スートの組み合わせ)に対し、各数字についてペアを探す。 */
  for (int i = 0; i < Hand::kPairFilterSize.at(pair_qty - 1); i++) {
    for (int j = 0; j < 15; j++){
      const Cards::bitcards tmpfilter{Hand::kPairFilters.at(pair_qty - 1).at(i) << j};
      if ((src & tmpfilter) == tmpfilter) {
        const Cards::bitcards j{};
        hand_vec.push_back(Hand::bitcards2Hand(tmpfilter, j));
        src ^= tmpfilter; // srcから手を構成するカード(=tmpfilterと一致する部分)を除く。
      }
    }
  }
}

void uecda::Hand::pushSequenceAndTakeAwayFromSrc(Cards::bitcards& src, std::vector<Hand> &hand_vec, const int seq_qty) {
  if (seq_qty < 3 || seq_qty > 14) { return; } // フィルターを定義していないseq_qtyが来たら何もしない。

  /* 各スートに対し、各数字に対して階段を探す。 */
  for(int i = 0; i < 4; i++) {
    for(int j = 0; j <= 15 - seq_qty; j++) {
      /* filterを、i番目のスートでj番目から始めるものに適用できるようシフトする。 */
      const Cards::bitcards tmpfilter{Hand::kSequenceFilters.at(seq_qty - 1) << (15 * i + j)};
      if((src & tmpfilter) == tmpfilter) { 
        const Cards::bitcards j{};
        hand_vec.push_back(Hand::bitcards2Hand(tmpfilter, j));
        src ^= tmpfilter; // srcから手を構成するカード(=tmpfilterと一致する部分)を除く。
      }
    }
  }
}
