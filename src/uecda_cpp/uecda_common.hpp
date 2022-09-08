#ifndef UECDA_COMMON_HPP_
#define UECDA_COMMON_HPP_

#include <array>
#include <cstdint>
#include <iostream>

namespace uecda {
  namespace common {
    using CommunicationBody = std::array<std::array<uint32_t, 15>, 8>;
  }

  inline std::ostream& operator<<(std::ostream& os, const common::CommunicationBody& src) {
    os << "-----------------------------------------------------------------------------" << std::endl;
    os << " |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 |" << std::endl;
    os << "-----------------------------------------------------------------------------" << std::endl;
    for (int i = 0; i < 8; i++) {
      os << i << "|";
      for (int j = 0; j < 15; j++) {
        os << "  ";
        if (src.at(i).at(j) == 0) {
          os << " ";
        } else {
          os << src.at(i).at(j);
        }
        os << " |";
      }
      os << std::endl;
      os << "-----------------------------------------------------------------------------" << std::endl;
    }
    return os;
  }
}

#endif // UECDA_COMMON_HPP_
