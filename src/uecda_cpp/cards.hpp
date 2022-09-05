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
    static constexpr int kSpade = 0;
    static constexpr int kHeart = 1;
    static constexpr int kDiamond = 2;
    static constexpr int kClover = 3;

    /* 空のカード。 */
    Cards(): cards_() {}

    /* 配列形式のカードから生成。 */
    Cards(uecda::common::CommunicationBody src);

    /* ビットカードから生成。 */
    Cards(bitcards src) { this->cards_ = src; }

    /* 与えられたビットカードの枚数を返す。 */
    static int count(bitcards src);

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

    /* デバッグ出力用に配列にカードを埋め込む。 */
    void putCards(uecda::common::CommunicationBody& dst) const;

    /* デバッグ用にカードを出力。 */
    void print();

    /* 与えられたカードを追加する。 */
    Cards& operator +=(const Cards &src) {
      this->cards_ |= src.cards_;
      return *this;
    }

    /* 与えられたカードとの和からなるオブジェクトを返す。 */
    Cards operator +(const Cards &src) const {
      Cards dst = *this;
      dst.cards_ |= src.cards_;
      return dst;
    }

    /* 与えられたカードを引く。 */
    Cards& operator -=(const Cards &src) {
      this->cards_ ^= (this->cards_ & src.cards_); 
      return *this;
    }

    /* 与えられたカードとの差からなるオブジェクトを返す。 */
    Cards operator -(const Cards &src) const {
      Cards dst = *this;
      dst.cards_ ^= (dst.cards_ & src.cards_); 
      return dst;
    }

    /* 等しいか。 */
    bool operator ==(const Cards &src) const {
      return this->cards_ == src.cards_;
    }

    /* 等しくないか。 */
    bool operator !=(const Cards &src) const {
      return !(*this == src);
    }

    /* 与えられたカードをすべて含むか。 */
    bool hasAllOf(Cards c) const {
      return (c - *this).quantity() == 0;
    }

    /* 与えられたカードを1枚以上含むか。 */
    bool hasAnyOf(Cards c) const {
      return (c.quantity() == 0) || (*this - c) != *this;
    }

    /* カードに与えられたフィルターをかけた結果を返す。 */
    bitcards filterCards(bitcards filter) const {
      return (this->cards_ & filter);
    }

    /* ジョーカーを除く。ジョーカーはワイルドカードでどかしづらいため。 */
    void deleteJoker() {
      this->cards_ &= 0xfffffffffffffff;
    }

   private:
    bitcards cards_;
  };
}

#endif  // CARDS_HPP_
