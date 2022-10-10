#ifndef CARDS_HPP_
#define CARDS_HPP_

#include <unistd.h>
#include <iostream>
#include <vector>

#include "uecda_common.hpp"

namespace uecda {
  class Cards {
   public:
    using bitcards = uint_fast64_t;
    using card_order = uint_fast16_t;

    /* 整数は、UECda-2007における標準ルール下でのカード表現と対応している。 */
    static constexpr int kSpade{0};
    static constexpr int kHeart{1};
    static constexpr int kDiamond{2};
    static constexpr int kClover{3};

    static constexpr bitcards JOKER {(bitcards)0b1'000000000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S3 {(bitcards)0b010000000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S4 {(bitcards)0b001000000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S5 {(bitcards)0b000100000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S6 {(bitcards)0b000010000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S7 {(bitcards)0b000001000000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S8 {(bitcards)0b000000100000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S9 {(bitcards)0b000000010000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S10{(bitcards)0b000000001000000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S11{(bitcards)0b000000000100000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S12{(bitcards)0b000000000010000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S13{(bitcards)0b000000000001000'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S1 {(bitcards)0b000000000000100'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards S2 {(bitcards)0b000000000000010'000000000000000'000000000000000'000000000000000};
    static constexpr bitcards H3 {(bitcards)0b010000000000000'000000000000000'000000000000000};
    static constexpr bitcards H4 {(bitcards)0b001000000000000'000000000000000'000000000000000};
    static constexpr bitcards H5 {(bitcards)0b000100000000000'000000000000000'000000000000000};
    static constexpr bitcards H6 {(bitcards)0b000010000000000'000000000000000'000000000000000};
    static constexpr bitcards H7 {(bitcards)0b000001000000000'000000000000000'000000000000000};
    static constexpr bitcards H8 {(bitcards)0b000000100000000'000000000000000'000000000000000};
    static constexpr bitcards H9 {(bitcards)0b000000010000000'000000000000000'000000000000000};
    static constexpr bitcards H10{(bitcards)0b000000001000000'000000000000000'000000000000000};
    static constexpr bitcards H11{(bitcards)0b000000000100000'000000000000000'000000000000000};
    static constexpr bitcards H12{(bitcards)0b000000000010000'000000000000000'000000000000000};
    static constexpr bitcards H13{(bitcards)0b000000000001000'000000000000000'000000000000000};
    static constexpr bitcards H1 {(bitcards)0b000000000000100'000000000000000'000000000000000};
    static constexpr bitcards H2 {(bitcards)0b000000000000010'000000000000000'000000000000000};
    static constexpr bitcards D3 {(bitcards)0b010000000000000'000000000000000};
    static constexpr bitcards D4 {(bitcards)0b001000000000000'000000000000000};
    static constexpr bitcards D5 {(bitcards)0b000100000000000'000000000000000};
    static constexpr bitcards D6 {(bitcards)0b000010000000000'000000000000000};
    static constexpr bitcards D7 {(bitcards)0b000001000000000'000000000000000};
    static constexpr bitcards D8 {(bitcards)0b000000100000000'000000000000000};
    static constexpr bitcards D9 {(bitcards)0b000000010000000'000000000000000};
    static constexpr bitcards D10{(bitcards)0b000000001000000'000000000000000};
    static constexpr bitcards D11{(bitcards)0b000000000100000'000000000000000};
    static constexpr bitcards D12{(bitcards)0b000000000010000'000000000000000};
    static constexpr bitcards D13{(bitcards)0b000000000001000'000000000000000};
    static constexpr bitcards D1 {(bitcards)0b000000000000100'000000000000000};
    static constexpr bitcards D2 {(bitcards)0b000000000000010'000000000000000};
    static constexpr bitcards C3 {(bitcards)0b010000000000000};
    static constexpr bitcards C4 {(bitcards)0b001000000000000};
    static constexpr bitcards C5 {(bitcards)0b000100000000000};
    static constexpr bitcards C6 {(bitcards)0b000010000000000};
    static constexpr bitcards C7 {(bitcards)0b000001000000000};
    static constexpr bitcards C8 {(bitcards)0b000000100000000};
    static constexpr bitcards C9 {(bitcards)0b000000010000000};
    static constexpr bitcards C10{(bitcards)0b000000001000000};
    static constexpr bitcards C11{(bitcards)0b000000000100000};
    static constexpr bitcards C12{(bitcards)0b000000000010000};
    static constexpr bitcards C13{(bitcards)0b000000000001000};
    static constexpr bitcards C1 {(bitcards)0b000000000000100};
    static constexpr bitcards C2 {(bitcards)0b000000000000010};
    static constexpr bitcards kSpadeCards {S3 | S4 | S5 | S6 | S7 | S8 | S9 | S10 | S11 | S12 | S1 | S2 };
    static constexpr bitcards kHeartCards {H3 | H4 | H5 | H6 | H7 | H8 | H9 | H10 | H11 | H12 | H1 | H2 };
    static constexpr bitcards kDiamondCards {D3 | D4 | D5 | D6 | D7 | D8 | D9 | D10 | D11 | D12 | D1 | D2 };
    static constexpr bitcards kCloverCards {C3 | C4 | C5 | C6 | C7 | C8 | C9 | C10 | C11 | C12 | C1 | C2 };

    static Cards communicationBody2Cards(const uecda::common::CommunicationBody& src);

    /* カード全体。 */
    static Cards all() {
      return Cards((bitcards)0b1'011111111111110'011111111111110'011111111111110'011111111111110);
    }

    /* 空のカード。 */
    constexpr Cards() {}

    /* ビットカードから生成。 */
    constexpr Cards(const bitcards src) { cards_ = src; }

    /* 与えられたビットカードの枚数を返す。 */
    static int count(const bitcards src);

    /* カードを返す。 */
    constexpr bitcards toBitcards() const { return cards_; }

    /* カードに含まれるスートを4bit形式で返す。 */
    int getSuits() const;

    /* カードの枚数を返す。 */
    int quantity() const;

    /* ジョーカーが含まれるか否かを返す。 */
    constexpr bool hasJoker() const { return (cards_ >> 60) == 1; }

    /* 最弱のカードを15bit形式で返す。 */
    card_order weakestOrder() const;

    /* 最強のカードを15bit形式で返す。 */
    card_order strongestOrder() const;

    /* カードを1つずつに分ける */
    std::vector<Cards> devideIntoOneCards() {
      bitcards src{cards_};
      bitcards cur{(bitcards)1};
      std::vector<Cards> result{};
    
      /* Joker以外の各札。 */
      for (int i = 3; i >= 0; i--) {
        for (int j = 14; j >= 0; j--) {
          if (src % 2 == 1) {
            result.push_back(Cards(cur));
          }
          src >>= 1;
          cur <<= 1;
        }
      }
      /* Joker。 */
      if (src % 2 == 1) {
        result.push_back(Cards(cur));
      }

      return result;
    }

    /* デバッグ出力用に配列にカードを埋め込む。 */
    void putCards(uecda::common::CommunicationBody& dst) const;

    /* デバッグ用にカードを出力。 */
    void print();

    /* 与えられたカードを追加する。 */
    constexpr Cards& operator +=(const Cards& src) { cards_ |= src.cards_; return *this; }

    /* 与えられたカードとの和からなるオブジェクトを返す。 */
    constexpr Cards operator +(const Cards& src) const { return cards_ | src.cards_; }

    /* 与えられたカードを引く。 */
    constexpr Cards& operator -=(const Cards& src) { cards_ ^= (cards_ & src.cards_); return *this; }

    /* 与えられたカードとの差からなるオブジェクトを返す。 */
    constexpr Cards operator -(const Cards& src) const { return cards_ ^ (cards_ & src.cards_); }

    /* 等しいか。 */
    constexpr bool operator ==(const Cards& src) const { return cards_ == src.cards_; }

    /* 等しくないか。 */
    constexpr bool operator !=(const Cards& src) const { return !(*this == src); }

    constexpr bool isEmpty() const { return cards_ == (bitcards)0; }

    /* 与えられたカードをすべて含むか。 */
    constexpr bool hasAllOf(const Cards& c) const { return (c - *this).isEmpty(); }

    /* 与えられたカードを1枚以上含むか。 */
    constexpr bool hasAnyOf(const Cards& c) const { return c.isEmpty() || (*this - c) != *this; }

    /* カードに与えられたフィルターをかけた結果を返す。 */
    constexpr bitcards filterCards(const bitcards filter) const { return (cards_ & filter); }

    /* ジョーカーを除く。ジョーカーはワイルドカードでどかしづらいため。 */
    constexpr void deleteJoker() { cards_ ^= JOKER; }

   private:
    bitcards cards_{};

    friend std::ostream& operator<<(std::ostream& os, const Cards& src) {
      common::CommunicationBody tmp{};
      src.putCards(tmp);

      os << "-----------------------------------------------------------------------------" << std::endl;
      os << " |  * |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 |  J |  Q |  K |  1 |  2 |  * |" << std::endl;
      os << "-----------------------------------------------------------------------------" << std::endl;
      for (int suit = 0; suit < 5; suit++) {
        switch (suit) {
          case Cards::kSpade:
            os << "S|";
            break;
          case Cards::kHeart:
            os << "H|";
            break;
          case Cards::kDiamond:
            os << "D|";
            break;
          case Cards::kClover:
            os << "C|";
            break;
          default:
            os << "*|";
            break;
        }
        for (int order = 0; order < 15; order++) {
          os << "  ";
          if (tmp.at(suit).at(order) == 1) {
            os << 1;
          } else if (tmp.at(suit).at(order) == 2) {
            os << 2;
          } else {
            std:: cout << " ";
          }
          os << " |";
        }
        os << std::endl;
        os << "-----------------------------------------------------------------------------" << std::endl;
      }
      return os;
    }
  };
}

#endif  // CARDS_HPP_
