#include "uecda_state.hpp"

UECdaState UECdaState::simulatePass() const {
  UECdaState result = UECdaState(*this);

  /* 全員がパスし終わったら、最後にパスした人(=現状態におけるプレイヤ)からトリック開始。 */
  bool has_all_passed = std::all_of(record_.has_passed.begin(), record_.has_passed.end(), true);
  if (has_all_passed) {
    result.finishTrick();
    result.last_action_ = uecda::Hand();
  } else {
    result.last_action_ = uecda::Hand();
    result.table_.whose_turn = this->nextPlayerNum();
  }

  return result;
}

UECdaState UECdaState::simulateSubmission(const uecda::Hand& hand) const {
  UECdaState result = UECdaState(*this);

  result.table_hand_ = hand;
  result.table_.whose_turn = this->nextPlayerNum();
  result.record_.last_submitted_player = this->cur_player_num_;
  result.table_.is_start_of_trick = false;
  result.last_action_ = hand;

  /* 提出者が出したカードを除く。 */
  result.player_cards_.at(cur_player_num_) -= hand.getCards();
  if (hand.getJoker() != uecda::Cards()) {
    result.player_cards_.at(table_.whose_turn).deleteJoker();
  }
  result.table_.card_quantity_of_players.at(table_.whose_turn) -= hand.getSummary().quantity;

  /* 上がりの処理。 */
  if (result.table_.card_quantity_of_players.at(table_.whose_turn) == 0) {
    result.table_.is_out.at(table_.whose_turn) = true;
    result.next_ranks_.at(table_.whose_turn) = *std::max_element(next_ranks_.begin(), next_ranks_.end()) + 1;
    /* 上がっていないのが1人だけなら、その人は大貧民。 */
    if (std::count(result.table_.is_out.begin(), result.table_.is_out.end(), true) == result.table_.is_out.size() - 1) {
      int last_player_num = std::find(result.table_.is_out.begin(), result.table_.is_out.end(), false) - result.table_.is_out.begin();
      result.table_.is_out.fill(true);
      result.next_ranks_.at(last_player_num) = *std::max_element(next_ranks_.begin(), next_ranks_.end()) + 1;
    }
  }
  
  /* 縛りの処理。 */
  if (this->submissionCausesLock(hand)) {
    result.table_.is_lock = true;
  }

  /* 革命の処理。 */
  if (this->submissionCausesRevolution(hand)) {
    result.table_.is_rev = !result.table_.is_rev;
  }

  /* 8切・スぺ3返しの処理。 */
  if (this->submissionIs8Giri(hand) || this->submissionIsSpade3Gaeshi(hand)) {
    result.finishTrick();
    if (!result.table_.is_out[cur_player_num_]) {
      result.table_.whose_turn = cur_player_num_;
    }
  }

  return result;
}
