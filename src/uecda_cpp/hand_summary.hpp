#ifndef HAND_SUMMARY_HPP_
#define HAND_SUMMARY_HPP_

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
  };
}

#endif  // HAND_SUMMARY_HPP_
