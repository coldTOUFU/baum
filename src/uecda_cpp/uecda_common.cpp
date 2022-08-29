#include "uecda_common.hpp"

void uecda::common::printCommunicationBody(CommunicationBody src) {
  std::cout << "-----------------------------------------------------------------------------\n"
               " |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 | 12 | 13 | 14 |\n"
               "-----------------------------------------------------------------------------\n";
  for (int i = 0; i < 8; i++) {
    std::cout << i << "|";

    for (int j = 0; j < 15; j++) {
      std::cout << "  ";
      if (src[i][j] == 0) {
        std::cout << " ";
      } else {
        std::cout << src[i][j];
      }
      std::cout << " |";
    }
    std::cout << std::endl;
    std::cout << "-----------------------------------------------------------------------------\n";
  }
}