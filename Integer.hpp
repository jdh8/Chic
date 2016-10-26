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

#include <limits>
#include <type_traits>
#include <cstdlib>

namespace Chic {

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

#endif // CHIC_INTEGER_HPP
