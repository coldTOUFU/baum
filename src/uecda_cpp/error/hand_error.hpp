#ifndef HAND_ERROR_
#define HAND_ERROR_

#include <stdexcept>

class CannotConvertToHandException: public std::exception {};

class TableHandIsNullException: public std::exception {};

#endif // HAND_ERROR_
