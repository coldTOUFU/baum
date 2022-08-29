#ifndef UECDA_CLIENT_ERROR_
#define UECDA_CLIENT_ERROR_

#include <stdexcept>

class OpenSocketException: public std::exception {};
class CloseSocketException: public std::exception {};
class ResolveServerException: public std::exception {};
class ConnectToServerException: public std::exception {};
class ReceivePlayerNumException: public std::exception {};
class ReceiveAcceptedFlagException: public std::exception {};
class ReceiveGameFinishStateException: public std::exception {};
class ReceiveCommunicationBodyException: public std::exception {};
class SendCommunicationBodyException: public std::exception {};
class UnexpectedGameFinishStateException: public std::exception {};

#endif // UECDA_CLIENT_ERROR_