#include "uecda_state.hpp"

UECdaState UECdaState::simulatePass() const {
  UECdaState result = UECdaState(*this);

  // TODO: 前に出した人がパスした場合、次の人が2回目にパスするとその人のターン。
  if (record_.last_submitted_player == cur_player_num_) {
    result.table_hand_ = uecda::Hand();
    result.table_.is_lock = false;
    result.table_.is_start_of_trick = true;
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
  // TODO: 最後の1人が残ったら、その人は大貧民。
  if (result.table_.card_quantity_of_players.at(table_.whose_turn) == 0) {
    result.table_.is_out.at(table_.whose_turn) = true;
    result.next_ranks_.at(table_.whose_turn) = *std::max_element(next_ranks_.begin(), next_ranks_.end()) + 1;
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
    result.table_hand_ = uecda::Hand();
    result.table_.is_lock = false;
    result.table_.is_start_of_trick = true;
    if (!result.table_.is_out[cur_player_num_]) {
      result.table_.whose_turn = cur_player_num_;
    }
  }

  return result;
}
