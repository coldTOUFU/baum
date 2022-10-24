#ifndef HAND_SUMMARY_HPP_
#define HAND_SUMMARY_HPP_

#include <bitset>

#include "cards.hpp"

namespace uecda {

  struct HandSummary {
    int quantity;
    bool is_pass;
    bool is_sequence;
    Cards::card_order weakest_order;
    Cards::card_order strongest_order;
    bool has_joker;
    int suits;

    constexpr HandSummary() : quantity(), is_pass(true), is_sequence(), weakest_order(), strongest_order(), has_joker(), suits() {}

    constexpr HandSummary(int q, bool is_p, bool is_s, Cards::card_order w_ord, Cards::card_order s_ord, bool has_j, int s) :
        quantity(q), is_pass(is_p), is_sequence(is_s), weakest_order(w_ord), strongest_order(s_ord), has_joker(has_j), suits(s) {}

    void print() const {
      std::cout << *this;
    }

    constexpr bool operator ==(const HandSummary &src) const {
      return quantity == src.quantity &&
          is_pass == src.is_pass &&
          is_sequence == src.is_sequence &&
          weakest_order == src.weakest_order &&
          strongest_order == src.strongest_order &&
          has_joker == src.has_joker &&
          suits == src.suits;
    }

    friend std::ostream& operator<<(std::ostream& os, const HandSummary& src) {
      os << "HandSummary" << std::endl;
      os << "  枚数: " << src.quantity << std::endl;
      os << "  パス？: " << (src.is_pass ? "YES" : "NO") << std::endl;
      os << "  階段？: " << (src.is_sequence) << std::endl;
      os << "  最弱のカード: " << std::bitset<15>(src.weakest_order) << std::endl;
      os << "  最強のカード: " << std::bitset<15>(src.strongest_order) << std::endl;
      os << "  ジョーカーあり？: " << (src.has_joker ? "YES" : "NO") << std::endl;
      os << "  スート: " << std::bitset<4>(src.suits) << std::endl;
      return os;
    }
  };
}

#endif  // HAND_SUMMARY_HPP_
