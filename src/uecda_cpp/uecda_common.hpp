#ifndef UECDA_COMMON_HPP_
#define UECDA_COMMON_HPP_

#include <array>
#include <cstdint>
#include <iostream>

namespace uecda {
  namespace common {
    using CommunicationBody = std::array<std::array<uint32_t, 15>, 8>;
    void printCommunicationBody(CommunicationBody src);
 }
}

#endif // UECDA_COMMON_HPP_
