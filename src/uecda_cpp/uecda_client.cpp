#include "uecda_client.hpp"

const std::string uecda::UECdaClient::kDefaultServerHostname = "127.0.0.1";
const std::string uecda::UECdaClient::kDefaultPlayerName = "sample";

int uecda::UECdaClient::enterGame(void) {
  /* サーバと接続する。 */
  try {
    openSocket();
  } catch (const std::exception& e) {
    throw;
  }

  /* クライアントの情報を送信。 */
  this->sendClientProfile();

  /* 自身のプレイヤ番号をサーバから受け取る。 */
  int my_playernum;
  if (read(this->sockfd_, &my_playernum, sizeof(my_playernum)) > 0) {
    my_playernum = ntohl(my_playernum);
  } else {
    throw ReceivePlayerNumException();
  }

  return my_playernum;
}

void uecda::UECdaClient::receiveMyInitialCards(uecda::common::CommunicationBody dst) const {
  try {
    receiveCommunicationBody(dst);
  } catch (const ReceiveCommunicationBodyException& e) {
    std::cerr << "ラウンド開始時の手札を受け取れませんでした。\n";
    throw;
  }
}

void uecda::UECdaClient::receiveMyCards(uecda::common::CommunicationBody dst) const {
  try {
    receiveCommunicationBody(dst);
  } catch (const ReceiveCommunicationBodyException& e) {
    std::cerr << "手札の受信に失敗しました。\n";
    throw;
  }
}

void uecda::UECdaClient::sendExchangeCards(uecda::common::CommunicationBody src) const {
  try {
    sendCommunicationBody(src);
  } catch (const SendCommunicationBodyException& e) {
    std::cerr << "交換用カードの送信に失敗しました。\n";
    throw;
  }
}

bool uecda::UECdaClient::sendSubmissionCards(uecda::common::CommunicationBody src) const {
  try {
    sendCommunicationBody(src);
  } catch (const SendCommunicationBodyException& e) {
    std::cerr << "手札の送信に失敗しました。\n";
    throw;
  }

  int is_accepted;
  if ((read(this->sockfd_, &is_accepted, sizeof(is_accepted))) <= 0) {
    throw ReceiveAcceptedFlagException();
  }

  return ntohl(is_accepted) == 9;
}

uecda::UECdaClient::GAME_FINISH_STATE uecda::UECdaClient::receiveGameFinishState(void) const {
  int flag;
  /* ゲーム終了に関する情報を受け取る。 */
  if ((read(this->sockfd_, &flag, sizeof(flag))) <= 0) {
    std::cerr << "ゲームの終了判定を受け取れませんでした。\n";
    throw ReceiveGameFinishStateException();
  }
  flag = ntohl(flag);

  switch (flag) {
    case UECdaClient::kContinueCase:
      return UECdaClient::GAME_FINISH_STATE::kContinue;
    case UECdaClient::kRoundFinishCase:
      return UECdaClient::GAME_FINISH_STATE::kRoundFinish;
    case UECdaClient::kGameFinishCase:
      return UECdaClient::GAME_FINISH_STATE::kGameFinish;
    default:
      throw UnexpectedGameFinishStateException();
  }
}

/* 場札を受け取る */
void uecda::UECdaClient::receiveTableCards(uecda::common::CommunicationBody dst) const {
  try {
    receiveCommunicationBody(dst);
  } catch (const SendCommunicationBodyException& e) {
    std::cerr << "場札を受け取れませんでした。\n";
    throw;
  }
}

void uecda::UECdaClient::receiveCommunicationBody(uecda::common::CommunicationBody dst_table) const {
  uecda::common::CommunicationBody src_table = {{}};
  if (read(this->sockfd_, src_table, 4 * 8 * 15) <= 0) {
    throw ReceiveCommunicationBodyException();
  }

  /* テーブルの要素をネットワークオーダーから
     ホストオーダーに変換した上でコピー。 */
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 15; j++) {
      dst_table[i][j] = ntohl(src_table[i][j]);
    }
  }
}

void uecda::UECdaClient::sendCommunicationBody(uecda::common::CommunicationBody src_table) const {
  uecda::common::CommunicationBody dst_table = {{}};
  /* テーブルの要素をホストオーダーから
     ネットワークオーダーに変換した上でコピー。 */
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 15; j++) {
      dst_table[i][j] = htonl(src_table[i][j]);
    }
  }
  if (write(this->sockfd_, dst_table, 4 * 8 * 15) <= 0) {
    throw SendCommunicationBodyException();
  }
}

void uecda::UECdaClient::openSocket() {
  const char* server_addr = this->server_hostname_.c_str();

  /* ソケット生成。 */
  if ((this->sockfd_ = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    std::cerr << "ソケット生成に失敗しました: " << std::endl;
    throw OpenSocketException();
  }

  /* ポートとアドレスの設定。 */
  bzero((char*)&(this->client_addr_), sizeof(this->client_addr_));
  this->client_addr_.sin_family = PF_INET;
  this->client_addr_.sin_port = htons(this->port_);
  this->client_addr_.sin_addr.s_addr = inet_addr(server_addr);

  /* IPアドレスで指定されていないとき、ホスト名の解決を試みる。 */
  if (this->client_addr_.sin_addr.s_addr == 0xffffffff) {
    struct hostent* host;
    host = gethostbyname(this->server_hostname_.c_str());
    if (host == NULL) {
      std::cerr << "ホスト名解決に失敗しました: " << server_addr << std::endl;
      throw ResolveServerException();
    }
    this->client_addr_.sin_addr.s_addr = *(unsigned int*)host->h_addr_list[0];
  }

  /* サーバに接続する。 */
  if (connect(this->sockfd_, (struct sockaddr*)&(this->client_addr_),
              sizeof(this->client_addr_)) != 0) {
    std::cerr << "サーバ接続に失敗しました: " << server_addr << std::endl;
    throw ConnectToServerException();
  }
}

/* クライアントの情報を送信 */
void uecda::UECdaClient::sendClientProfile() const {
  uecda::common::CommunicationBody profile = {{}};
  profile[0][0] = UECdaClient::kProtocolVersion;
  for (int i = 0; i < 15; i++) {
    profile[1][i] = (int)this->player_name_[i];
  }

  try {
    sendCommunicationBody(profile);
  } catch (const SendCommunicationBodyException& e) {
    std::cerr << "プロファイルの送信に失敗しました。\n";
    throw;
  }
}
