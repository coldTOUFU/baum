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

    /* 整数は、UECda-2007における標準ルール下でのカード表現と対応している。 */
    static constexpr int kSpade{0};
    static constexpr int kHeart{1};
    static constexpr int kDiamond{2};
    static constexpr int kClover{3};

    static constexpr bitcards JOKER {(bitcards)0b100000000000000'000000000000000'000000000000000'000000000000000};
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

    /* カード全体。 */
    static Cards all() {
      return Cards((bitcards)0b1'011111111111110'011111111111110'011111111111110'011111111111110);
    }

    /* 空のカード。 */
    Cards() {}

    /* 配列形式のカードから生成。 */
    Cards(const uecda::common::CommunicationBody& src);

    /* ビットカードから生成。 */
    Cards(const bitcards src) { this->cards_ = src; }

    /* 与えられたビットカードの枚数を返す。 */
    static int count(const bitcards src);

    /* カードを返す。 */
    bitcards toBitcards() const { return this->cards_; }

    /* カードに含まれるスートを4bit形式で返す。 */
    int getSuits() const;

    /* カードの枚数を返す。 */
    int quantity() const;

    /* ジョーカーが含まれるか否かを返す。 */
    bool hasJoker() const { return (this->cards_ >> 60) == 1; }

    /* 最弱のカードを15bit形式で返す。 */
    bitcards weakestOrder() const;

    /* 最強のカードを15bit形式で返す。 */
    bitcards strongestOrder() const;

    /* カードを1つずつに分ける */
    std::vector<Cards> devideIntoOneCards() {
      bitcards src{this->cards_};
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
    Cards& operator +=(const Cards& src) {
      this->cards_ |= src.cards_;
      return *this;
    }

    /* 与えられたカードとの和からなるオブジェクトを返す。 */
    Cards operator +(const Cards& src) const {
      Cards dst{*this};
      dst.cards_ |= src.cards_;
      return dst;
    }

    /* 与えられたカードを引く。 */
    Cards& operator -=(const Cards& src) {
      this->cards_ ^= (this->cards_ & src.cards_); 
      return *this;
    }

    /* 与えられたカードとの差からなるオブジェクトを返す。 */
    Cards operator -(const Cards& src) const {
      Cards dst{*this};
      dst.cards_ ^= (dst.cards_ & src.cards_); 
      return dst;
    }

    /* 等しいか。 */
    bool operator ==(const Cards& src) const {
      return this->cards_ == src.cards_;
    }

    /* 等しくないか。 */
    bool operator !=(const Cards& src) const {
      return !(*this == src);
    }

    /* 与えられたカードをすべて含むか。 */
    bool hasAllOf(const Cards& c) const {
      return (c - *this).quantity() == 0;
    }

    /* 与えられたカードを1枚以上含むか。 */
    bool hasAnyOf(const Cards& c) const {
      return (c.quantity() == 0) || (*this - c) != *this;
    }

    /* カードに与えられたフィルターをかけた結果を返す。 */
    bitcards filterCards(const bitcards filter) const {
      return (this->cards_ & filter);
    }

    /* ジョーカーを除く。ジョーカーはワイルドカードでどかしづらいため。 */
    void deleteJoker() {
      this->cards_ &= (bitcards)0xfffffffffffffff;
    }

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
