// This file is part of Chic, a Tchisla solver.
//
// Copyright (C) 2016 Chen-Pang He <https://jdh8.org/>
//
// Chic is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Chic is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef CHIC_INTEGER_HPP
#define CHIC_INTEGER_HPP

#include "Arithmetic.hpp"
#include <functional>
#include <limits>
#include <vector>
#include <sstream>
#include <cmath>

namespace Chic {
/*!
 * \brief Integer with exact arithmetic
 *
 * \tparam Unsigned  Underlying unsigned type
 *
 * Exact elementary arithmetic, exponentiation, and square root are provided.
 * If there is no representable exact answer, 0 is returned.
 *
 * Unsigned integer overflow wraps around, but signed integer overflow causes
 * undefined behavior.  Therefore, the underlying type must be unsigned.
 */
template<typename Unsigned>
class Integer : public Arithmetic<Integer<Unsigned>>
{
  private:
    Unsigned _value;

  public:
    Integer(const Unsigned& = 0);
    Integer(std::size_t, int);

    const Unsigned& value() const { return _value; }

    explicit operator bool() const { return _value; }

    Integer& operator*=(bool condition) { _value *= condition; return *this; }

    Integer& operator++() { ++_value; return *this; }
    Integer& operator--() { --_value; return *this; }

    Integer& operator+=(const Integer&);
    Integer& operator-=(const Integer&);
    Integer& operator*=(const Integer&);
    Integer& operator/=(const Integer&);
};

template<typename Unsigned>
Integer<Unsigned>::Integer(const Unsigned& value)
  : _value(value)
{
  typedef std::numeric_limits<Unsigned> Traits;
  static_assert(Traits::is_integer && !Traits::is_signed, "The underlying type must be an unsigned integer.");
}

/*!
 * \brief Construct a repeating number
 *
 * For example, 3333 can be constructed with Integer(4, 3).
 *
 * This special constructor is provided because Tchisla allows digit
 * concatenation.  The signature of this constructor resembles
 * std::string::string(size_t, char) to provide a consistent interface.
 */
template<typename Unsigned>
Integer<Unsigned>::Integer(std::size_t repeats, int digit)
  : _value(0)
{
  while (repeats--)
    _value = 10 * _value + digit;
}

/*!
 * \brief Exact addition
 *
 * Overflow causes inexact result, so then 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator+=(const Integer& other)
{
  _value += other._value;
  _value *= other._value <= _value;

  return *this;
}

/*!
 * \brief Exact subtraction
 *
 * Underflow causes inexact result, so then 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator-=(const Integer& other)
{
  _value = (_value >= other._value) * (_value - other._value);

  return *this;
}

/*!
 * \brief Exact multiplication
 *
 * Overflow causes inexact result, so then 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator*=(const Integer& other)
{
  Unsigned result = _value * other._value;

  _value = (_value && result / _value == other._value) * result;

  return *this;
}

/*!
 * \brief Exact division
 *
 * If there is no exact quotient, 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator/=(const Integer& other)
{
  _value = (other._value && _value % other._value == 0) * (_value / other._value);

  return *this;
}

template<typename Unsigned>
Integer<Unsigned> operator*(Integer<Unsigned> x, bool condition)
{
  return x *= condition;
}

template<typename Unsigned>
Integer<Unsigned> operator*(bool condition, Integer<Unsigned> x)
{
  return x *= condition;
}

template<typename Unsigned>
bool operator==(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
{
  return x.value() == y.value();
}

template<typename Unsigned>
bool operator<(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
{
  return x.value() < y.value();
}

template<typename Character, typename Unsigned>
std::basic_ostream<Character>& operator<<(std::basic_ostream<Character>& stream, const Integer<Unsigned>& integer)
{
  return stream << integer.value();
}

/*!
 * \brief Exact exponentiation
 *
 * Overflow causes inexact result, so then 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned> pow(Integer<Unsigned> base, const Integer<Unsigned>& y)
{
  Integer<Unsigned> result = 1;

  for (Unsigned exponent = y.value(); exponent; exponent >>= 1) {
    if (exponent & 1)
      result = result * base;
    base = base * base;
  }
  return result;
}

/*!
 * \brief Exact square root
 *
 * If the argument is no perfect square, 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned> sqrt(const Integer<Unsigned>& x)
{
  Unsigned result = std::sqrt(x.value());
  return (result * result == x.value()) * result;
}

template<typename Unsigned>
class Factorial
{
  private:
    std::vector<Integer<Unsigned>> _table;

  public:
    Factorial();
    Integer<Unsigned> operator()(const Integer<Unsigned>&) const;
};

template<typename Unsigned>
Factorial<Unsigned>::Factorial()
  : _table(1, 1)
{
  for (Integer<Unsigned> k = 1; ; ++k) {
    Integer<Unsigned> candidate = k * _table.back();

    if (candidate)
      _table.push_back(candidate);
    else
      break;
  }
}

template<typename Unsigned>
Integer<Unsigned> Factorial<Unsigned>::operator()(const Integer<Unsigned>& n) const
{
  return n.value() < _table.size() ? _table[n.value()] : 0;
}

/*!
 * \brief Exact factorial
 *
 * Overflow causes inexact result, so then 0 is returned.
 */
template<typename Unsigned>
Integer<Unsigned> factorial(const Integer<Unsigned>& n)
{
  static const Factorial<Unsigned> implementation;
  return implementation(n);
}

namespace detail {

#ifdef __BMI__

inline
int ctz(unsigned int x)
{
  return __builtin_ctz(x);
}

inline
int ctz(unsigned long x)
{
  return __builtin_ctzl(x);
}

inline
int ctz(unsigned long long x)
{
  return __builtin_ctzll(x);
}

template<typename Unsigned>
Unsigned gcd(Unsigned x, Unsigned y)
{
  if (!x) return y;

  int shift = ctz(x|y);

  x >>= ctz(x);

  while (y) {
    y >>= ctz(y);
    if (x > y) std::swap(x, y);
    y -= x;
  }

  return x << shift;
}

#else // __BMI__

template<typename Unsigned>
Unsigned gcd(Unsigned x, Unsigned y)
{
  while (y) {
    x %= y;
    std::swap(x, y);
  }

  return x;
}

#endif // __BMI__

} // namespace detail

template<typename Unsigned>
Integer<Unsigned> gcd(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
{
  return detail::gcd(x.value(), y.value());
}

template<typename Unsigned>
Integer<Unsigned> rotate(const Integer<Unsigned>& x, int shift)
{
  const int digits = std::numeric_limits<Unsigned>::digits;
  const unsigned int mask = digits - 1;

  static_assert((digits & -digits) == digits, "Digits must be a power of 2 to perform circular shift.");

  return (x.value() << shift) | (x.value() >> (-shift & mask));
}

} // namespace Chic

namespace std {

template<typename Unsigned>
struct hash<Chic::Integer<Unsigned>>
{
  std::size_t operator()(const Chic::Integer<Unsigned>& x) const { return x.value(); }
};

} // namespace std

#endif // CHIC_INTEGER_HPP
