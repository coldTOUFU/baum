#include "learn_playout_policy.hpp"

UECdaState readLog(std::ifstream& fstream) {
  int last_submitted_player;
  std::array<bool, 5> has_passed;
  fstream >> last_submitted_player;
  for (int i = 0; i < 5; i++) {
    int tmp;
    fstream >> tmp;
    has_passed.at(i) = tmp == 1;
  }  
  GameRecord record{last_submitted_player, has_passed};

  uecda::Cards::bitcards table_hand_bitcards, table_hand_bitjoker;
  fstream >> table_hand_bitcards >> table_hand_bitjoker;
  uecda::Hand table_hand{uecda::Hand::bitcards2Hand(table_hand_bitcards, table_hand_bitjoker)};

  bool is_my_turn;
  int whose_turn;
  bool is_start_of_trick;
  bool is_rev;
  bool is_lock;
  std::array<int, 5> card_quantity_of_players;
  std::array<bool, 5> is_out;
  std::array<int, 5> rank_of_players;
  std::array<int, 5> player_num_on_seats;
  fstream >> is_my_turn;
  fstream >> whose_turn;
  fstream >> is_start_of_trick;
  fstream >> is_rev;
  fstream >> is_lock;
  for (int i = 0; i < 5; i++) {
    fstream >> card_quantity_of_players.at(i);
  }  
  for (int i = 0; i < 5; i++) {
    int tmp;
    fstream >> tmp;
    is_out.at(i) = tmp == 1;
  }  
  for (int i = 0; i < 5; i++) {
    fstream >> rank_of_players.at(i);
  }  
  for (int i = 0; i < 5; i++) {
    int tmp;
    fstream >> tmp;
    player_num_on_seats.at(i) = tmp == 1;
  }
  uecda::Table table{
    is_my_turn,
    whose_turn,
    is_start_of_trick,
    is_rev,
    is_lock,
    card_quantity_of_players,
    is_out,
    rank_of_players,
    player_num_on_seats
  };

  std::array<uecda::Cards, 5> player_cards;
  for (int i = 0; i < 5; i++) {
    uecda::Cards::bitcards b;
    fstream >> b;
    player_cards.at(i) = uecda::Cards(b);
  }

  std::array<int, 5> next_ranks;
  for (int i = 0; i < 5; i++) {
    fstream >> next_ranks.at(i);
  }

  uecda::Cards::bitcards last_action_bitcards;
  uecda::Cards::bitcards last_action_bitjoker;
  fstream >> last_action_bitcards >> last_action_bitjoker;
  uecda::Hand last_action{uecda::Hand::bitcards2Hand(last_action_bitcards, last_action_bitjoker)};

  return {
    record,
    table_hand,
    table,
    player_cards,
    next_ranks,
    last_action
  };
}

SnowlPolicyVector gradientOfSoftMax(uecda::Cards my_cards, std::vector<uecda::Hand> hands, uecda::Hand hand, bool is_rev, SnowlPolicyVector weights) {
  const SnowlPolicyVector feature_vec{snowlFeatureVector(my_cards, hand, is_rev)};
  SnowlPolicyVector result{feature_vec};

  double softmax_denom{}; // softmax関数の分母。
  SnowlPolicyVector sum{};
  for (const uecda::Hand hand : hands) {
    double exp_eval{exp(snowlEvaluation(my_cards, hand, is_rev, weights))};
    softmax_denom += exp_eval;
    for (int i = 0; i < 166; i++) {
      sum.at(i) += exp_eval * feature_vec.at(i);
    }
  }

  for (int i = 0; i < 166; i++) {
    result.at(i) -= sum.at(i) / softmax_denom;
  }

  return result;
}

int main(void) {
  SnowlPolicyVector weights{};

  std::ifstream log_fstream;
  log_fstream.open("state_log.tsv", std::ios::in);
  std::ofstream weights_fstream;
  log_fstream.open("weights.tsv", std::ios::out);

  std::random_device seed_gen;

  while (!log_fstream.eof()) {
    UECdaState state{readLog(log_fstream)};
    const int my_playernum{state.getTable().whose_turn};

    /* 現在のパラメータでの平均得点を求める。 */
    /* 1回のプレイアウトで高々1e6の乱数が消費されるので、kPlayoutSizeが高々1e4なら乱数は消費し尽くさないはず。
       そのためループの外で乱数エンジンを初期化してseed_gen()の計算時間を節約する。 */
    double score_sum{};
    XorShift64 random_engine{seed_gen()};
    for (int i = 0; i < kPlayoutSize; i++) {
      UECdaState tmp_state{state};
      while (!tmp_state.isFinished()) {
        tmp_state = tmp_state.next(snowlPlayoutPolicy(state, random_engine, weights));
      }
      score_sum += tmp_state.getScore(my_playernum);
    }
    const double score_mean{score_sum / kPlayoutSize};

    /* 教師での平均得点を求める。教師はランダムプレイアウトを用いたモンテカルロ木探索を行う。 */
    double supervisor_score_sum{};
    for (int i = 0; i < kPlayoutSize; i++) {
      UECdaState tmp_state{state};
      while (!tmp_state.isFinished()) {
        random_engine = XorShift64(seed_gen());
        tmp_state = tmp_state.next(supervisorPlayoutPolicy(tmp_state, random_engine));
      }
      supervisor_score_sum += tmp_state.getScore(my_playernum);
    }
    const double supervisor_score_mean{supervisor_score_sum / kPlayoutSize};

    /* 現在のパラメータでの平均勾配を求める。 */
    random_engine = XorShift64(seed_gen());
    SnowlPolicyVector grad_mean{};
    for (int i = 0; i < kPlayoutSize; i++) {
      UECdaState tmp_state{state};
      int num_of_turn{}; // 自分が着手(意思決定)した回数。
      SnowlPolicyVector softmax_gradient_sum{};

      while (!tmp_state.isFinished()) {
        uecda::Hand next_hand{snowlPlayoutPolicy(tmp_state, random_engine, weights)};

        /* 自分の手番かつ、選択の余地がある場合のみパラメータ更新に考慮。 */
        if (tmp_state.getTable().whose_turn == my_playernum
            && tmp_state.legalActions().size() > 1) {
          const uecda::Cards tmp_my_cards{tmp_state.getPlayerCards().at(my_playernum)};
          bool is_rev{tmp_state.getTable().is_rev};
          num_of_turn++;
          SnowlPolicyVector softmax_gradient{gradientOfSoftMax(tmp_my_cards, tmp_state.legalActions(), next_hand, is_rev, weights)};
          for (int j = 0; j < 166; j++) {
            softmax_gradient_sum.at(j) += softmax_gradient.at(j);
          }
        }

        tmp_state = tmp_state.next(next_hand);
      }

      for (int j = 0; j < 166; j++) {
        grad_mean.at(j) += softmax_gradient_sum.at(j) / (kPlayoutSize * num_of_turn);
      }
    }

    /* パラメータ更新。 */
    for (int i = 0; i < 166; i++) {
      weights.at(i) += (supervisor_score_mean - score_mean) * grad_mean.at(i);
    }
  }
  // 学習した勾配を記録。
  for (int i = 0; i < 166; i++) {
    weights_fstream << weights.at(i);
    if (i < 165) {
      weights_fstream << "\t";
    }
  }

  log_fstream.close();
  weights_fstream.close();
}
