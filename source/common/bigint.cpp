#include "common/bigint.hpp"
/*
BigInt BigInt::operator+(const BigInt &other) const {
  BigInt ret = other;
  return ret += *this;
}
BigInt BigInt::operator-(const BigInt &other) const {
  BigInt ret = other;
  return ret -= *this;
}
BigInt BigInt::operator*(const BigInt &other) const {
  BigInt ret = other;
  return ret *= *this;
}
BigInt BigInt::operator/(const BigInt &other) const {
  BigInt ret = other;
  return ret /= *this;
}
BigInt BigInt::operator%(const BigInt &other) const {
  BigInt ret = other;
  return ret %= *this;
}

BigInt BigInt::operator+() const { return std::copy(*this); }
BigInt BigInt::operator-() const { return ++(~*this); } // something like this?
BigInt &BigInt::operator++() { return *this += 1; }
BigInt BigInt::operator++(int) {
  BigInt ret = *this;
  ++*this;
  return ret;
}
BigInt &BigInt::operator--() { return *this -= 1; }
BigInt BigInt::operator--(int) {
  BigInt ret = *this;
  ++*this;
  return ret;
}

bool BigInt::operator==(const BigInt &other) const {
  for (szd::size_t i = 0; i < intTypeCount; ++i) {
    if (getByte(i) != other.getByte(i)) {
      return false;
    }
  }
  return true;
}
bool BigInt::operator==(const BigInt &other) const {
  return !((*this) == other);
}
bool BigInt::operator<(const BigInt &other) const {
  for (szd::size_t i = intTypeCount; i > 0; --i) {
    if (getByte(i - 1) != other.getByte(i - 1)) {
      return getByte(i - 1) < other.getByte(i - 1));
    }
  }
  return false;
}
bool BigInt::operator>=(const BigInt &other) const {
  return ((*this) == other) || ((*this) > other);
}
bool BigInt::operator<=(const BigInt &other) const {
  return ((*this) == other) || ((*this) < other);
}
bool BigInt::operator>(const BigInt &other) const {
  return !((*this) < other);
}*/
