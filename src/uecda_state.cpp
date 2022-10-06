#include "uecda_state.hpp"

UECdaState UECdaState::simulatePass() const {
  UECdaState result{*this};

  result.record_.has_passed.at(table_.whose_turn) = true;
  const bool has_all_passed{std::all_of(result.record_.has_passed.begin(), result.record_.has_passed.end(), [](const auto& e) { return e; })};

  /* 全員がパスし終わったら、新しいトリックを開始。 */
  if (has_all_passed) {
    result.finishTrick();
    result.last_action_ = uecda::Hand();
    /* 最後に出したプレイヤが上がった場合、その次のプレイヤから始める。そうでないなら、出してから一周してるはずなので今のプレイヤから始める。 */
    if (table_.is_out.at(record_.last_submitted_player)) {
      result.table_.whose_turn = result.nextPlayerNumOf(record_.last_submitted_player);
    }
  } else {
    result.last_action_ = uecda::Hand();
    result.table_.whose_turn = result.nextPlayerNumOf(table_.whose_turn);
  }

  return result;
}

UECdaState UECdaState::simulateSubmission(const uecda::Hand& hand) const {
  UECdaState result{*this};

  result.table_hand_ = hand;
  result.table_.whose_turn = this->nextPlayerNumOf(table_.whose_turn);
  result.record_.last_submitted_player = this->table_.whose_turn;
  result.table_.is_start_of_trick = false;
  result.last_action_ = hand;

  /* 提出者が出したカードを除く。 */
  result.player_cards_.at(table_.whose_turn) -= hand.getCards();
  if (hand.getSummary().has_joker) {
    result.player_cards_.at(table_.whose_turn).deleteJoker();
  }
  result.table_.card_quantity_of_players.at(table_.whose_turn) -= hand.getSummary().quantity;

  /* 上がりの処理。 */
  if (result.table_.card_quantity_of_players.at(table_.whose_turn) == 0) {
    result.table_.is_out.at(table_.whose_turn) = true;
    result.record_.has_passed.at(table_.whose_turn) = true;
    result.next_ranks_.at(table_.whose_turn) = *std::max_element(next_ranks_.begin(), next_ranks_.end()) + 1;
    /* 上がっていないのが1人だけなら、その人を大貧民にしてゲームを終わらせる。 */
    if (std::count(result.table_.is_out.begin(), result.table_.is_out.end(), true) == result.table_.is_out.size() - 1) {
      int last_player_num = std::find(result.table_.is_out.begin(), result.table_.is_out.end(), false) - result.table_.is_out.begin();
      result.table_.is_out.fill(true);
      result.record_.has_passed.fill(true);
      result.next_ranks_.at(last_player_num) = *std::max_element(result.next_ranks_.begin(), result.next_ranks_.end()) + 1;
      return result;
    }

    /* 他の全員がパスの状態で自分が上がったら、新しいトリックを始める。 */
    const bool has_all_passed{std::all_of(result.record_.has_passed.begin(), result.record_.has_passed.end(), [](const auto& e) { return e; })};
    if (has_all_passed) {
      result.finishTrick();
      result.table_.whose_turn = result.nextPlayerNumOf(table_.whose_turn);
      return result;
    }
  }
  
  /* 縛りの処理。 */
  result.table_.is_lock = this->submissionCausesLock(hand);

  /* 革命の処理。 */
  if (this->submissionCausesRevolution(hand)) {
    result.table_.is_rev = !result.table_.is_rev;
  }

  /* 8切・スぺ3返しの処理。 */
  if (this->submissionIs8Giri(hand) || this->submissionIsSpade3Gaeshi(hand)) {
    result.finishTrick();
    if (!result.table_.is_out[table_.whose_turn]) {
      result.table_.whose_turn = table_.whose_turn;
    }
  }

  return result;
}
