#ifndef UECDA_CLIENT_HPP_
#define UECDA_CLIENT_HPP_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "error/uecda_client_error.hpp"
#include "uecda_common.hpp"

namespace uecda {
  class UECdaClient {
   public:
    enum class GAME_FINISH_STATE { kContinue, kRoundFinish, kGameFinish };

    /* クライアントの初期化。通信用テーブルサイズの関係でプレイヤ名は15文字以内に訂正する。 */
    UECdaClient(std::string pname = kDefaultPlayerName, int port = kDefaultPort, std::string sname = kDefaultServerHostname)
        : server_hostname_(sname), port_(port), player_name_(pname.substr(0, 15)) {}

    /* サーバに接続してゲームに参加する。自分のプレーヤ番号を返す。 */
    int enterGame();

    void exitGame() const {
      /* サーバとの接続を切る。 */
      try {
        closeSocket();
      } catch (const std::exception& e) {
        throw;
      }
    }

    /* サーバからラウンド開始時の手札を受け取る。 */
    void receiveMyInitialCards(uecda::common::CommunicationBody& dst) const;

    /* カード交換時のカードの提出 */
    void sendExchangeCards(const uecda::common::CommunicationBody& cards) const;

    /* カードを受け取る。 */
    void receiveMyCards(uecda::common::CommunicationBody& dst) const;

    /* カードを提出し受理されたか否かを返す */
    bool sendSubmissionCards(const uecda::common::CommunicationBody& src) const;

    //ラウンドの最後にゲームか終ったかを、サーバから受けとりその値を返す。
    GAME_FINISH_STATE receiveGameFinishState(void) const;

    /* 場札を受け取る */
    void receiveTableCards(uecda::common::CommunicationBody& dst) const;

   private:
    static constexpr int kProtocolVersion{20070};
    static constexpr int kDefaultPort{42485};
    static const std::string kDefaultServerHostname;
    static const std::string kDefaultPlayerName;
    /* UECda-2007のプロトコルにおける、ゲーム終了フラグの設定と対応。
       http://www.tnlab.inf.uec.ac.jp/daihinmin/2021/document_protocol.htmlでは
       ゲーム継続時の値が明記されていないが、標準クライアントでは0の場合でswitchしているので、
       0で継続ということにする。*/
    static constexpr int kContinueCase{0};
    static constexpr int kRoundFinishCase{1};
    static constexpr int kGameFinishCase{2};

    int sockfd_;
    struct sockaddr_in client_addr_;
    const std::string server_hostname_;
    const int port_;
    const std::string player_name_;

    /* ソケットの初期化を行う。 */
    void openSocket();

    /* 通信を切断する。 */
    void closeSocket() const {
      if (close(sockfd_) != 0) {
        std::cerr << "サーバ切断に失敗しました: " << std::endl;
        throw CloseSocketException();
      }
    }

    /* クライアントの情報を送信。 */
    void sendClientProfile() const;

    /* サーバからカードを受け取ってdst_tableに埋め込む。 */
    void receiveCommunicationBody(uecda::common::CommunicationBody& dst) const;

    /* サーバからカードを受け取ってdst_tableに埋め込む。 */
    void sendCommunicationBody(const uecda::common::CommunicationBody& src) const;
  };
}

#endif  // UECDA_CLIENT_HPP_
