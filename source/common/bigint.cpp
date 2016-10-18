#include "common/bigint.hpp"

// template <std::size_t size, typename intType, std::size_t intTypeSize>
// using BigIntType = BigInt<size, intType, intTypeSize>;

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator+(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret += other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator-(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret -= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator*(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret *= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator/(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret /= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator%(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret %= other;
}


template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator+() const {
  return std::copy(*this);
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator-() const {
  return ++(~*this);// something like this?
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator++() {
  return *this += 1;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator++(int) {
  BigInt ret = *this;
  ++*this;
  return ret;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator--() {
  return *this -= 1;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator--(int) {
  BigInt ret = *this;
  --*this;
  return ret;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator==(const BigInt<size, intType, intTypeSize> &other) const {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    if (getByte(i) != other.getByte(i)) {
      return false;
    }
  }
  return true;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator!=(const BigInt<size, intType, intTypeSize> &other) const {
  return !(*this == other);
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator<(const BigInt<size, intType, intTypeSize> &other) const {
  for (std::size_t i = intTypeCount; i > 0; --i) {
    if (getByte(i - 1) != other.getByte(i - 1)) {
      return getByte(i - 1) < other.getByte(i - 1);
    }
  }
  return false;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator>(const BigInt<size, intType, intTypeSize> &other) const {
  return !((*this == other) || (*this < other));
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator<=(const BigInt<size, intType, intTypeSize> &other) const {
  return (*this == other) || (*this < other);
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
bool BigInt<size, intType, intTypeSize>::
operator>=(const BigInt<size, intType, intTypeSize> &other) const {
  return (*this == other) || (*this > other);
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator~() const {
  BigInt<size, intType, intTypeSize> ret = *this;
  for (auto &i : ret._data) {
    i = ~i;
  }
  return ret;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator|(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret |= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator&(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret &= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator^(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret ^= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator<<(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret <<= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> BigInt<size, intType, intTypeSize>::
operator>>(const BigInt<size, intType, intTypeSize> &other) const {
  BigInt<size, intType, intTypeSize> ret = *this;
  return ret >>= other;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator+=(const BigInt<size, intType, intTypeSize> &other) {
  bool carry = false;
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] += other._data[i];
    if (carry) {
      ++_data[i];
    }
    carry = (_data[i] < other._data[i]);
  }
  return *this;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator-=(const BigInt<size, intType, intTypeSize> &other) {
  return *this += -other;// not perfectly optimized
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator*=(const BigInt<size, intType, intTypeSize> &other) {
  return mul(other).first;// not perfectly optimized
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator/=(const BigInt<size, intType, intTypeSize> &other) {
  return div(other).first;// not perfectly optimized
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator%=(const BigInt<size, intType, intTypeSize> &other) {
  return div(other).second;// not perfectly optimized
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator&=(const BigInt<size, intType, intTypeSize> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] &= other._data[i];
  }
  return *this;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator|=(const BigInt<size, intType, intTypeSize> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] |= other._data[i];
  }
  return *this;
}

template <std::size_t size, typename intType, std::size_t intTypeSize>
BigInt<size, intType, intTypeSize> &BigInt<size, intType, intTypeSize>::
operator^=(const BigInt<size, intType, intTypeSize> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] ^= other._data[i];
  }
  return *this;
}
