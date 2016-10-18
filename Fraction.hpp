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

#ifndef CHIC_FRACTION_HPP
#define CHIC_FRACTION_HPP

#include "Integer.hpp"

namespace Chic {
/*!
 * \brief Nonnegative fraction with exact arithmetic
 *
 * \tparam Unsigned  Underlying unsigned type
 *
 * Exact elementary arithmetic, exponentiation, and square root are provided.
 * If an overflow occurs, infinity is returned.  If the result is otherwise
 * not representable, nan is returned.
 *
 * Unsigned integer overflow wraps around, but signed integer overflow causes
 * undefined behavior.  Therefore, the underlying type must be unsigned.
 */
template<typename Unsigned>
class Fraction : Base<Fraction<Unsigned>>
{
  private:
    Unsigned _num;
    Unsigned _den;

  public:
    Fraction(const Unsigned& = 0, const Unsigned& = 1);

    void canonicalize();

    Fraction inverse() const;

    Integer<Unsigned> num() const { return _num; }
    Integer<Unsigned> den() const { return _den; }
    
    template<typename Character>
    std::basic_string<Character> str() const;

    std::string str() const;

    operator bool() const { return _num && _den; }

    Fraction& operator+=(const Fraction&);
    Fraction& operator-=(const Fraction&);
    Fraction& operator*=(const Fraction&);
    Fraction& operator/=(const Fraction&);
};

template<typename Unsigned>
Fraction<Unsigned>::Fraction(const Unsigned& num, const Unsigned& den)
  : _num(num),
    _den(den)
{
  canonicalize();
}

template<typename Unsigned>
void Fraction<Unsigned>::canonicalize()
{
  Unsigned divisor = gcd(_num, _den);

  _num /= divisor;
  _den /= divisor;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::inverse() const
{
  Fraction<Unsigned> result;

  result._num = _den;
  result._den = _num;

  return result;
}

template<typename Unsigned>
template<typename Character>
std::basic_string<Character> Fraction<Unsigned>::str() const
{
  std::basic_ostringstream<Character> stream;
  stream << _num << '/' << _den;
  return stream.str();
}

template<typename Unsigned>
std::string Fraction<Unsigned>::str() const
{
  return str<char>();
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator+=(const Fraction& other)
{
  Fraction multiplier(_den, other._den);

  _num = (num() * multiplier.den() + other.num() * multiplier.num()).value();
  _den = (den() * multiplier.den()).value();

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator-=(const Fraction& other)
{
  Fraction multiplier(_den, other._den);

  _num = (num() * multiplier.den() - other.num() * multiplier.num()).value();
  _den = (den() * multiplier.den()).value();

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(const Fraction& other)
{
  Fraction a(_num, other._den);
  Fraction b(other._num, _den);

  _num = (a.num() * b.num()).value();
  _den = (a.den() * b.den()).value();

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator/=(const Fraction& other)
{
  return *this *= other.inverse();
}

} // namespace Chic

#endif // CHIC_FRACTION_HPP
