#ifdef YesIWannaReallyIncludeTheBigInt
#undef YesIWannaReallyIncludeTheBigInt
// template <std::size_t size, bool isSigned, typename intType>
// using BigIntType = BigInt<size, isSigned, intType>;

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator+(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret += other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator-(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret -= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator*(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret *= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator/(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret /= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator%(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret %= other;
}


template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator+() const {
  return std::copy(*this);
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator-() const {
  return ++(~*this);// something like this?
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator++() {
  return *this += 1;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator++(int) {
  BigInt ret = *this;
  ++*this;
  return ret;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator--() {
  return *this -= 1;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator--(int) {
  BigInt ret = *this;
  --*this;
  return ret;
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator==(const BigInt<size, isSigned, intType> &other) const {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    if (getByte(i) != other.getByte(i)) {
      return false;
    }
  }
  return true;
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator!=(const BigInt<size, isSigned, intType> &other) const {
  return !(*this == other);
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator<(const BigInt<size, isSigned, intType> &other) const {
  for (std::size_t i = intTypeCount; i > 0; --i) {
    if (getByte(i - 1) != other.getByte(i - 1)) {
      return getByte(i - 1) < other.getByte(i - 1);
    }
  }
  return false;
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator>(const BigInt<size, isSigned, intType> &other) const {
  return !((*this == other) || (*this < other));
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator<=(const BigInt<size, isSigned, intType> &other) const {
  return (*this == other) || (*this < other);
}

template <std::size_t size, bool isSigned, typename intType>
bool BigInt<size, isSigned, intType>::
operator>=(const BigInt<size, isSigned, intType> &other) const {
  return (*this == other) || (*this > other);
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator~() const {
  BigInt<size, isSigned, intType> ret = *this;
  for (auto &i : ret._data) {
    i = ~i;
  }
  return ret;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator|(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret |= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator&(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret &= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator^(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret ^= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator<<(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret <<= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> BigInt<size, isSigned, intType>::
operator>>(const BigInt<size, isSigned, intType> &other) const {
  BigInt<size, isSigned, intType> ret = *this;
  return ret >>= other;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator+=(const BigInt<size, isSigned, intType> &other) {
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

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator-=(const BigInt<size, isSigned, intType> &other) {
  return *this += -other;// not perfectly optimized
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator*=(const BigInt<size, isSigned, intType> &other) {
  return mul(other).first;// not perfectly optimized
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator/=(const BigInt<size, isSigned, intType> &other) {
  return div(other).first;// not perfectly optimized
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator%=(const BigInt<size, isSigned, intType> &other) {
  return div(other).second;// not perfectly optimized
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator&=(const BigInt<size, isSigned, intType> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] &= other._data[i];
  }
  return *this;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator|=(const BigInt<size, isSigned, intType> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] |= other._data[i];
  }
  return *this;
}

template <std::size_t size, bool isSigned, typename intType>
BigInt<size, isSigned, intType> &BigInt<size, isSigned, intType>::
operator^=(const BigInt<size, isSigned, intType> &other) {
  for (std::size_t i = 0; i < intTypeCount; ++i) {
    _data[i] ^= other._data[i];
  }
  return *this;
}

#endif
