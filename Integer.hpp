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
#include "Overflow.hpp"
#include "Factorial.hpp"
#include <cmath>

namespace Chic {

template<typename Unsigned>
class Integer : public Arithmetic<Integer<Unsigned>>
{
  private:
    Overflow<Unsigned> _value;

  public:
    Integer(Unsigned = 0);
    Integer(std::size_t, int);

    operator Unsigned() const;
    Unsigned value() const;

    Integer& operator*=(bool);

    Integer& operator+=(Integer);
    Integer& operator-=(Integer);
    Integer& operator*=(Integer);
    Integer& operator/=(Integer);

    Integer pow(Unsigned) const;
    Integer sqrt() const;
    Integer factorial() const;
    Integer factorial(Integer) const;
};

template<typename Unsigned>
Integer<Unsigned>::Integer(Unsigned value)
  : _value(value)
{}

template<typename Unsigned>
Integer<Unsigned>::Integer(std::size_t repeats, int digit)
  : _value(0)
{
  while (repeats--)
    _value = 10 * _value + digit;
}

template<typename Unsigned>
Integer<Unsigned>::operator Unsigned() const
{
  return _value;
}

template<typename Unsigned>
Unsigned Integer<Unsigned>::value() const
{
  return _value;
}

template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator*=(bool condition)
{
  _value *= condition;
  return *this;
}

template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator+=(Integer other)
{
  _value *= !(_value += other.value());
  return *this;
}

template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator-=(Integer other)
{
  _value *= !(_value -= other.value());
  return *this;
}

template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator*=(Integer other)
{
  _value *= !(_value *= other.value());
  return *this;
}

template<typename Unsigned>
Integer<Unsigned>& Integer<Unsigned>::operator/=(Integer other)
{
  Unsigned result = _value / other._value;

  _value = result * (other._value * result == _value);

  return *this;
}

template<typename Unsigned, typename Other>
Integer<Unsigned> operator*(Integer<Unsigned> x, Other other)
{
  return x *= other;
}

template<typename Unsigned>
Integer<Unsigned> operator*(bool condition, Integer<Unsigned> x)
{
  return x *= condition;
}

template<typename Unsigned>
Integer<Unsigned> Integer<Unsigned>::pow(Unsigned exponent) const
{
  Integer base = *this;
  Integer result = 1;

  for (; exponent; exponent >>= 1) {
    if (exponent & 1)
      result *= base;
    base *= base;
  }
  return result;
}

template<typename Unsigned>
Integer<Unsigned> Integer<Unsigned>::sqrt() const
{
  Unsigned result = std::sqrt(value());
  return (result * result == value()) * result;
}

template<typename Unsigned>
Integer<Unsigned> Integer<Unsigned>::factorial() const
{
  return Chic::factorial(_value);
}

template<typename Unsigned>
Integer<Unsigned> Integer<Unsigned>::factorial(Integer lesser) const
{
  Integer result = *this >= lesser;

  for (Integer multiplier = _value; multiplier > lesser; --multiplier)
    result *= multiplier;

  return result;
}

namespace detail {

template<typename Integer>
Integer abs(Integer x, std::false_type)
{
  return x;
}

template<typename Integer>
typename std::make_unsigned<Integer>::type abs(Integer x, std::true_type)
{
  return std::abs(x);
}

template<typename Integer>
typename std::make_unsigned<Integer>::type abs(Integer x)
{
  return abs(x, std::integral_constant<bool, std::numeric_limits<Integer>::is_signed>());
}

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

#ifdef __BMI__

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

template<typename T>
T gcd(T x, T y)
{
  return detail::gcd(detail::abs(x), detail::abs(y));
}

template<typename Unsigned>
Unsigned rotate(Unsigned x, int shift, std::false_type)
{
  const int digits = std::numeric_limits<Unsigned>::digits;
  const unsigned int mask = digits - 1;

  static_assert((digits & -digits) == digits, "Digits must be a power of 2 to perform circular shift.");

  return (x << shift) | (x >> (-shift & mask));
}

template<typename Unsigned>
Unsigned rotate(Unsigned x, int shift)
{
  return rotate(x, shift, std::integral_constant<bool, std::numeric_limits<Unsigned>::is_signed>());
}

} // namespace Chic

namespace std {

template<typename Unsigned>
struct hash<Chic::Integer<Unsigned>>
{
  std::size_t operator()(const Chic::Integer<Unsigned>& x) const { return x; }
};

} // namespace std

#endif // CHIC_INTEGER_HPP
