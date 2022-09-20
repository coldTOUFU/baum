#include "cards.hpp"

uecda::Cards::Cards(const uecda::common::CommunicationBody& src) {
  this->cards_ = 0;

  /* Joker。 */
  if (src.at(4).at(1) == 2) {
    this->cards_++;
  }
  this->cards_ <<= 1;

  /* Joker以外の各札。 */
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 15; j++) {
      if (src.at(i).at(j) == 1) {
        this->cards_++;
      }
      this->cards_ <<= 1;
    }
  }

  /* 上で余計に1回左シフトした分を戻す。 */
  this->cards_ >>= 1;
}

int uecda::Cards::getSuits() const {
  bitcards tmp{this->cards_};
  tmp &= (bitcards)0xfffffffffffffff; // Jokerをビット列から落とす。

  int s{0};

  for (int i = 0; i < 4; i++) {
    bitcards filter{(bitcards)0xffff >> 1}; // 1-15ビット目が1。
    filter <<= 15 * (3 - i); // 目的のスートのカードのみフィルタリングできるようにシフトする。
    s += (tmp & filter) != 0;
    s <<= 1;
  }
  s >>= 1; // 一回余分にシフトした分を戻す。
  return s;
}

int uecda::Cards::quantity() const {
  return this->count(this->cards_);
}

int uecda::Cards::count(const bitcards src) {
  /*
    32bitsの場合の例。
      tmp = 0b10010010100100011001001010110001
    とする。
      tmp = (tmp & 0x5555555555555555) + (tmp >> 1 & 0x5555555555555555)
    の結果は、
      1010001010100010101000101100001
    となる。
    元のtmpと比べると、
      10|01|00|10|10|01|00|01|10|01|00|10|10|11|00|01
      01|01|00|01|01|01|00|01|01|01|00|01|01|10|00|01
    のように、2bits区切で1を数えているのがわかる。
  */
  bitcards tmp{src};
  tmp = (tmp & 0x5555555555555555) + (tmp >> 1 & 0x5555555555555555);   // 2bits区切でビット数を数える。
  tmp = (tmp & 0x3333333333333333) + (tmp >> 2 & 0x3333333333333333);   // 4bits区切。
  tmp = (tmp & 0x0f0f0f0f0f0f0f0f) + (tmp >> 4 & 0x0f0f0f0f0f0f0f0f);   // 8bits。
  tmp = (tmp & 0x00ff00ff00ff00ff) + (tmp >> 8 & 0x00ff00ff00ff00ff);   // 16bits。
  tmp = (tmp & 0x0000ffff0000ffff) + (tmp >> 16 & 0x0000ffff0000ffff);  // 32bits。
  return (tmp & 0x00000000ffffffff) + (tmp >> 32 & 0x00000000ffffffff); // 64bits。
}

uecda::Cards::bitcards uecda::Cards::weakestOrder() const {
  bitcards tmp{this->cards_};
  tmp &= (bitcards)0xfffffffffffffff; // Jokerをビット列から落とす。

  /*
    一番左(弱い)の1がどのスートの位置にあるかわからないので、
    15ずつずらして論理和をとり、2^0から2^14の範囲内に収める。
    一番左の1がクローバー以外のスートにある場合は2^15以降の範囲に
    1が立っているので、これを落とす。
  */
  tmp = (tmp | (tmp >> 15) | (tmp >> 30) | (tmp >> 45)) & (0xffff >> 1);

  /* 一番左の1が立っているビットから右をすべて1にする。 */
  tmp |= (tmp >> 1); // 1が1つ右隣に伝播する(eg. 01000000 -> 01100000)。
  tmp |= (tmp >> 2); // 1が2つ右隣に伝播する(eg. 01100000 -> 01111000)。
  tmp |= (tmp >> 4); // 1が4つ右隣に伝播する。
  tmp |= (tmp >> 8);

  /* 一番左の1以外を落とす。 */
  tmp ^= (tmp >> 1);

  return tmp;
}

uecda::Cards::bitcards uecda::Cards::strongestOrder() const {
  bitcards tmp{this->cards_};
  tmp &= (bitcards)0xfffffffffffffff; // Jokerをビット列から落とす。

  /*
    一番右(強い)の1がどのスートの位置にあるかわからないので、
    15ずつずらして論理和をとり、2^0から2^14の範囲内に収める。
    一番右の1がクローバー以外のスートにある場合は2^15以降の範囲に
    1が立っているので、これを落とす。
  */
  tmp = (tmp | (tmp >> 15) | (tmp >> 30) | (tmp >> 45)) & (0xffff >> 1);

  /* 一番右のbitだけ立てて返す。 */
  return tmp & (-tmp);
}

void uecda::Cards::putCards(uecda::common::CommunicationBody &dst) const {
  bitcards src{this->cards_};

  /* Joker以外の各札。 */
  for (int i = 3; i >= 0; i--) {
    for (int j = 14; j >= 0; j--) {
      dst.at(i).at(j) = src % 2;
      src >>= 1;
    }
  }
  /* Joker。 */
  dst.at(4).at(1) = (src % 2) * 2;
}


void uecda::Cards::print() {
  std::cout << *this;
}
