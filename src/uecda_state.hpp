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
  UECdaState()
      : record_(),
        table_hand_(),
        table_(true, 0, true, false, false, {}, {}, {}, {}),
        player_cards_(),
        next_ranks_(),
        last_action_() {
    /* UECdaStateでは、提出処理を常に受け付けたいのでtrueとする。 */
    table_.is_my_turn = true;
  }

  UECdaState(GameRecord& record,
             uecda::Hand& table_hand,
             uecda::Table& table,
             std::array<uecda::Cards, 5>& player_cards,
             std::array<int, 5>& next_ranks,
             uecda::Hand& last_action)
      : record_(record),
        table_hand_(table_hand),
        table_(table),
        player_cards_(player_cards),
        next_ranks_(next_ranks),
        last_action_(last_action) {
    /* UECdaStateでは、提出処理を常に受け付けたいのでtrueとする。 */
    table_.is_my_turn = true;
  }

  /* 受け取った手を適用して得られる状態を返す。 */
  UECdaState next(const uecda::Hand& hand) const {
    if (hand.getSummary().is_pass || !this->isLegal(hand)) {
      return this->simulatePass();
    } else {
      return this->simulateSubmission(hand);
    }
  }

  /* 合法手の全体を返す。 */
  std::vector<uecda::Hand> legalActions() const {
    std::vector<uecda::Hand> dst{};
    uecda::Hand::pushLegalHands(player_cards_.at(table_.whose_turn), dst, table_, table_hand_);
    /* 場にカードがある場合は、パスも合法手に含める。ない場合は、無意味なので含めない。 */
    if (!table_.is_start_of_trick) {
      dst.push_back(uecda::Hand());
    }
    return dst;
  }

  /* ゲームが終了しているか？ */
  bool isFinished() const {
    return std::all_of(table_.is_out.begin(), table_.is_out.end(), [](const auto& e) { return e; });
  }

  /* 指定されたプレイヤ番号の現時点での得点を返す。上がっていなければ0。 */
  double getScore(const int player_num) const {
    if (next_ranks_.at(player_num) < 0) {
      return 0.0;
    } else {
      return (5.0 - next_ranks_.at(player_num)) / 5.0;
    }
  }

  void print() const {
    std::cout << *this;
  }

  constexpr UECdaState& operator =(const UECdaState& src) {
    record_ = src.record_;
    table_hand_ = src.table_hand_;
    table_ = src.table_;
    player_cards_ = src.player_cards_;
    next_ranks_ = src.next_ranks_;
    last_action_ = src.last_action_;
    return *this;
  }

  bool operator ==(const UECdaState& src) const {
    return record_ == src.record_ &&
        table_hand_ == src.table_hand_ &&
        table_ == src.table_ &&
        std::equal(player_cards_.begin(), player_cards_.end(), src.player_cards_.begin()) &&
        std::equal(next_ranks_.begin(), next_ranks_.end(), src.next_ranks_.begin()) &&
        last_action_ == src.last_action_;
  }

  int getMyPlayerNum() const { return table_.whose_turn; }

  GameRecord getRecord() const { return record_; }

  uecda::Hand getTableHand() const { return table_hand_; }

  uecda::Table getTable() const { return table_; }

  std::array<uecda::Cards, 5> getPlayerCards() const { return player_cards_; }

  std::array<int, 5> getNextRanks() const { return next_ranks_; }

  uecda::Hand getLastAction() const { return last_action_; }

 private:
  GameRecord record_;                        // ゲームの時系列情報。
  uecda::Hand table_hand_;                   // 場の手。
  uecda::Table table_;                       // 場の状態。
  std::array<uecda::Cards, 5> player_cards_; // 各プレイヤの手札(プレイヤ番号でアクセス)。
  std::array<int, 5> next_ranks_;            // 次ゲームでの階級(プレイヤ番号でアクセス)。0で大富豪、4で大貧民。まだ上がっていないなら-1。
  uecda::Hand last_action_;                  // 最後に打たれた手。

  bool isLegal(const uecda::Hand& hand) const {
    const uecda::Cards cards_of_cur_player = player_cards_.at(table_.whose_turn);
    return cards_of_cur_player.hasAllOf(hand.getCards()) && hand.isLegal(table_, table_hand_);
  }

  /* player_numのプレイヤの次に座っているパスしていないプレイヤ。 */
  int nextPlayerNumOf(const int player_num) const {
    const auto& num_on_seats{table_.player_num_on_seats};
    const auto cur_seat_itr = std::find(num_on_seats.begin(), num_on_seats.end(), player_num);
    const int cur_seat_idx{(int)std::distance(num_on_seats.begin(), cur_seat_itr)};

    /* 上がっていないプレイヤ内で次のプレイヤを探す。 */
    for (unsigned int i = 0; i < record_.has_passed.size(); i++) {
      const int next_player_num{num_on_seats.at((cur_seat_idx + i + 1) % num_on_seats.size())};
      if (!record_.has_passed.at(next_player_num)) {
        return next_player_num;
      }
    }
    return player_num;
  }

  bool submissionIs8Giri(const uecda::Hand& hand) const {
    constexpr uecda::Cards::bitcards eights{(uecda::Cards::bitcards)9007474141036800};
    return hand.getWholeBitcards().filterCards(eights) != (uecda::Cards::bitcards)0;
  }

  bool submissionIsSpade3Gaeshi(const uecda::Hand& hand) const {
    constexpr uecda::Cards::bitcards spade3{(uecda::Cards::bitcards)0b010000000000000 << 45};
    const uecda::HandSummary table_summary{table_hand_.getSummary()};
    return table_summary.has_joker && table_summary.quantity == 1 && hand.getWholeBitcards() == spade3;
  }

  bool submissionCausesLock(const uecda::Hand& hand) const {
    /* ジョーカー1枚出しの場合は縛りでないとする。 */
    if (hand.getSummary().quantity == 1 && hand.getSummary().has_joker) {
      return false;
    }
    return this->last_action_.getSummary().suits == hand.getSummary().suits;
  }

  bool submissionCausesRevolution(const uecda::Hand& hand) const {
    const uecda::HandSummary summary{hand.getSummary()};
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
    for (unsigned int i = 0; i < record_.has_passed.size(); i++) {
      record_.has_passed.at(i) = table_.is_out.at(i);
    }
  }

  UECdaState simulatePass() const;

  UECdaState simulateSubmission(const uecda::Hand& hand) const;

  friend std::ostream& operator<<(std::ostream& os, const UECdaState& src) {
    os << "## UECdaState" << std::endl;
    os << "# GameRecord" << std::endl;
    os << src.record_;
    os << "# 場の手" << std::endl;
    os << src.table_hand_;
    os << "# Table" << std::endl;
    os << src.table_;
    os << "# 各プレイヤの手札" << std::endl;
    for (const auto& cards: src.player_cards_) {
      os << cards;
    }
    os << "# 各プレイヤの次階級" << std::endl;
    for (const auto& rank: src.next_ranks_) {
      os << rank << " ";
    }
    os << std::endl;
    os << "# 最後の着手" << std::endl;
    os << src.last_action_;
    return os;
  }
};

#endif // UECDA_STATE_HPP_
