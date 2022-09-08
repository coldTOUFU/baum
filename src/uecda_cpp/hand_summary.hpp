#ifndef HAND_SUMMARY_HPP_
#define HAND_SUMMARY_HPP_

#include <bitset>

#include "cards.hpp"

namespace uecda {

  struct HandSummary {
    int quantity;
    bool is_pass;
    bool is_sequence;
    Cards::bitcards weakest_order;
    Cards::bitcards strongest_order;
    bool has_joker;
    int suits;

    void print() const {
      std::cout << *this;
    }

    bool operator ==(const HandSummary &src) const {
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
