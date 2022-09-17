#include "simulate_dealing.hpp"

void simulate_random_dealing(const int my_playernum, const uecda::Cards my_cards, std::array<uecda::Cards, 5>& player_cards, uecda::Cards rest_cards, const uecda::Table table) {
  std::random_device seed_gen;
  std::default_random_engine rand_engine(seed_gen());

  std::array<int, 5> rest_quantity_of_not_dealt_cards = table.card_quantity_of_players;

  player_cards.fill(uecda::Cards());
  player_cards.at(my_playernum) = my_cards;
  rest_quantity_of_not_dealt_cards.at(my_playernum) = 0;

  if (rest_cards.hasJoker()) {
    std::vector<int> players_able_to_have_joker{};
    for (int i = 0; i < 5; i++) {
      /* 交換があるので、ジョーカーは平民以上しか持ちえない。 */
      if (i != my_playernum && !table.is_out.at(i) && table.rank_of_players.at(i) < 3) {
        players_able_to_have_joker.push_back(i);
      }
    }

    std::uniform_int_distribution<int> dist_joker(0, players_able_to_have_joker.size() - 1);
    int random_playernum = dist_joker(rand_engine);
    player_cards.at(random_playernum) += uecda::Cards((uecda::Cards::bitcards)1 << 60);
    rest_quantity_of_not_dealt_cards.at(random_playernum) -= 1;
    rest_cards.deleteJoker();
  }

  std::vector<uecda::Cards> each_of_rest_cards = rest_cards.devideIntoOneCards();
  std::shuffle(each_of_rest_cards.begin(), each_of_rest_cards.end(), rand_engine);
  std::vector<uecda::Cards>::const_iterator begin_itr = each_of_rest_cards.cbegin();
  std::vector<uecda::Cards>::const_iterator end_itr;
  for (int i = 0; i < 5; i++) {
    if (rest_quantity_of_not_dealt_cards.at(i) <= 0) { continue; }

    end_itr = std::next(begin_itr, rest_quantity_of_not_dealt_cards.at(i));
    player_cards.at(i) += std::accumulate(begin_itr, end_itr, uecda::Cards{});
    begin_itr = end_itr;
  }
}
