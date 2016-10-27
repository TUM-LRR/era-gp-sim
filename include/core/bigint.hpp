#ifndef ERAGPSIM_CORE_BIGINT_HPP_
#define ERAGPSIM_CORE_BIGINT_HPP_

#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

#include <common/integer-information.hpp>

// gentlemans aggreement: only use unsigned stuff as intType, or expect
// unexpected behaviour
template <std::size_t size,
          bool isSigned = false,
          typename intType = std::uint64_t>
// std::enable_if<std::is_unsigned<intType>::value, >
class BigInt {
 public:
  // static_assert(std::is_unsigned<intType>::value);
  using BigIntType = BigInt<size, isSigned, intType>;
  BigInt() = default;
  BigInt(const BigIntType &other) = default;
  BigInt(BigIntType &&other) = default;
  BigIntType &operator=(const BigIntType &other) = default;
  BigIntType &operator=(BigIntType &&other) = default;
  // TODO::int constructors
  // binary operators
  BigIntType operator+(const BigIntType &other) const;
  BigIntType operator-(const BigIntType &other) const;
  BigIntType operator*(const BigIntType &other) const;
  BigIntType operator/(const BigIntType &other) const;
  BigIntType operator%(const BigIntType &other) const;
  // unary operators
  BigIntType operator+() const;
  BigIntType operator-() const;
  BigIntType &operator++();  //++a
  BigIntType operator++(int);// a++
  BigIntType &operator--();  //--a
  BigIntType operator--(int);// a--
  // comparison operators
  bool operator==(const BigIntType &other) const;
  bool operator!=(const BigIntType &other) const;
  bool operator>=(const BigIntType &other) const;
  bool operator<=(const BigIntType &other) const;
  bool operator>(const BigIntType &other) const;
  bool operator<(const BigIntType &other) const;
  // bit operators
  BigIntType operator~() const;
  BigIntType operator|(const BigIntType &other) const;
  BigIntType operator&(const BigIntType &other) const;
  BigIntType operator^(const BigIntType &other) const;
  BigIntType operator<<(const BigIntType &other) const;
  BigIntType operator>>(const BigIntType &other) const;
  // compound operators
  BigIntType &operator+=(const BigIntType &other);
  BigIntType &operator-=(const BigIntType &other);
  BigIntType &operator*=(const BigIntType &other);
  BigIntType &operator/=(const BigIntType &other);
  BigIntType &operator%=(const BigIntType &other);
  BigIntType &operator&=(const BigIntType &other);
  BigIntType &operator|=(const BigIntType &other);
  BigIntType &operator^=(const BigIntType &other);
  BigIntType &operator<<=(const BigIntType &other);
  BigIntType &operator>>=(const BigIntType &other);
  // cross int support, this might actually be quite hard D:
  // it actually might be a pain D:
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator+=(T &other);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator-=(T &other);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator*=(T &other);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator/=(T &other);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator%=(T &other);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator<<=(T shift);
  template <typename T>
  typename std::enable_if<std::is_integral<T>::value, BigIntType>::type &
  operator>>=(T shift);
  BigIntType operator<<(std::size_t shift) const;
  BigIntType operator>>(std::size_t shift) const;

  operator bool();
  // sar, sal, ror, rol
  // stream stuff

  // I possibly need stuff to handle BigInt<X> x BigInt<Y> what a pain

 private:
  static constexpr std::size_t intTypeSize =
      integerInformation<intType>::_numberOfBits;
  static constexpr std::size_t intTypeCount =
      (size + (intTypeSize - 1)) / intTypeSize;
  // std::vector<intType> _data(intTypeCount, static_cast<intType>(0));
  std::vector<intType> _data{};

  intType getByte(std::size_t index);

  // first->lower; second->higher
  std::pair<BigIntType, BigIntType> mul(const BigIntType &other) const;
  // first->div; second->mod
  std::pair<BigIntType, BigIntType> div(const BigIntType &other) const;
  std::vector<BigIntType> pow(const BigIntType &other) const;
};

#define YesIWannaReallyIncludeTheBigInt
#include "core/bigint-include.hpp"
#endif
