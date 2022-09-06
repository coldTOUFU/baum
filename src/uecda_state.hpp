#ifndef UECDA_STATE_HPP_
#define UECDA_STATE_HPP_

#include <algorithm>
#include <vector>

#include "game_record.hpp"
#include "uecda_cpp/cards.hpp"
#include "uecda_cpp/hand.hpp"
#include "uecda_cpp/table.hpp"

class UECdaState {
 public:
  UECdaState(GameRecord record,
             uecda::Hand table_hand,
             uecda::Table table,
             std::array<uecda::Cards, 5> player_cards,
             std::array<int, 5> next_ranks,
             uecda::Hand last_action):
      record_(record),
      table_hand_(table_hand),
      table_(table),
      player_cards_(player_cards),
      next_ranks_(next_ranks),
      last_action_(last_action) {}

  /* 受け取った手を適用して得られる状態を返す。 */
  UECdaState next(uecda::Hand const &hand) const {
    if (hand.getSummary().is_pass || this->isLegal(hand)) {
      return this->simulatePass();
    } else {
      return this->simulateSubmission(hand);
    }
  }

  /* 合法手の全体を返す。 */
  std::vector<uecda::Hand> legalActions() const {
    std::vector<uecda::Hand> dst;
    uecda::Hand::pushHands(player_cards_.at(cur_player_num_), dst);
    return dst;
  }

  /* ゲームが終了しているか？ */
  bool isFinished() const {
    return std::all_of(table_.is_out.begin(), table_.is_out.end(), true);
  }

  /* 指定されたプレイヤ番号の現時点での得点を返す。上がっていなければ0。 */
  int getScore(const int player_num) const {
    if (next_ranks_.at(player_num) < 0) {
      return 0;
    } else {
      return 5 - next_ranks_.at(player_num);
    }
  }

  /* 最後の着手(この盤面に至ったときの着手)を返す。 */
  uecda::Hand getLastAction() const {
    return last_action_;
  }
 
 private:
  GameRecord record_;                        // ゲームの時系列情報。
  uecda::Hand table_hand_;                   // 場の手。
  uecda::Table table_;                       // 場の状態。
  std::array<uecda::Cards, 5> player_cards_; // 各プレイヤの手札(プレイヤ番号でアクセス)。
  std::array<int, 5> next_ranks_;            // 次ゲームでの階級(プレイヤ番号でアクセス)。0で大富豪、4で大貧民。まだ上がっていないなら-1。
  uecda::Hand last_action_;                  // 最後に打たれた手。
  const int &cur_player_num_ = table_.whose_turn; // プレイヤ番号へのエイリアス。

  bool isLegal(const uecda::Hand hand) const {
    const uecda::Cards cards_of_cur_player = player_cards_.at(cur_player_num_);
    return cards_of_cur_player.hasAllOf(hand.getCards()) && hand.isLegal(table_, table_hand_);
  }

  int nextPlayerNum() const {
    auto& num_on_seats = table_.player_num_on_seats;
    int cur_seat_idx = std::find(num_on_seats.begin(), num_on_seats.end(), cur_player_num_) - num_on_seats.begin();

    /* 上がっていないプレイヤ内で次のプレイヤを探す。 */
    for (int i = 0; i < record_.has_passed.size(); i++) {
      int nextPlayerNum = num_on_seats.at((cur_seat_idx + i + 1) % num_on_seats.size());
      if (!record_.has_passed.at(nextPlayerNum)) {
        return nextPlayerNum;
      }
    }
    return cur_player_num_;
  }

  bool submissionIs8Giri(const uecda::Hand& hand) const {
    uecda::Cards::bitcards eights = (uecda::Cards::bitcards)9007474141036800;
    return hand.getWholeBitcards().filterCards(eights) != (uecda::Cards::bitcards)0;
  }

  bool submissionIsSpade3Gaeshi(const uecda::Hand& hand) const {
    uecda::Cards::bitcards spade3 = ((uecda::Cards::bitcards)0b010000000000000 << 45);
    uecda::HandSummary last_summary = last_action_.getSummary();
    return last_summary.has_joker && last_summary.quantity == 1 && hand.getWholeBitcards() == spade3;
  }

  bool submissionCausesLock(const uecda::Hand hand) const {
    return this->last_action_.getSummary().suits == hand.getSummary().suits;
  }

  bool submissionCausesRevolution(const uecda::Hand hand) const {
    const uecda::HandSummary& summary = hand.getSummary();
    if (summary.is_sequence) {
      return summary.quantity >= 5;
    } else {
      return summary.quantity >= 4;
    }
  }

  void finishTrick() {
    table_hand_ = uecda::Hand();
    table_.is_lock = false;
    table_.is_start_of_trick = true;
    /* 上がっているプレイヤはパス扱い。 */
    for (int i = 0; i < record_.has_passed.size(); i++) {
      record_.has_passed.at(i) = table_.is_out.at(i);
    }
  }

  UECdaState simulatePass() const;

  UECdaState simulateSubmission(const uecda::Hand& hand) const;
};

#endif // UECDA_STATE_HPP_
