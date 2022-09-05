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
      std::cout << "枚数: " << quantity << std::endl;
      std::cout << "パス？: " << (this->is_pass ? "YES" : "NO") << std::endl;
      std::cout << "階段？: " << (this->is_sequence) << std::endl;
      std::cout << "最弱のカード: " << std::bitset<15>(weakest_order) << std::endl;
      std::cout << "最強のカード: " << std::bitset<15>(strongest_order) << std::endl;
      std::cout << "ジョーカーあり？: " << (this->has_joker ? "YES" : "NO") << std::endl;
      std::cout << "スート: " << std::bitset<4>(suits) << std::endl;
    }
  };
}

#endif  // HAND_SUMMARY_HPP_
