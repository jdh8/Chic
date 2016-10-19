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
    Integer<Unsigned> _den;

  public:
    Fraction(const Unsigned& = 0, const Unsigned& = 1);

    /*!
     * \brief Numerator of the fraction
     *
     * Because zero is a valid numerator, its type is Unsigned instead of
     * Integer<Unsigned>.
     */
    const Unsigned& numerator() { return _num; }

    /*!
     * \brief Denominator of the fraction
     *
     * Integer<Unsigned> mixes zero with invalid representation.  This makes it
     * a perfect denominator because zero denominator is also invalid.
     */
    const Integer<Unsigned>& denominator() { return _den; }

    Fraction inverse() const;

    template<typename Character>
    std::basic_string<Character> str() const;

    std::string str() const;

    operator bool() const { return _num && _den; }

    Fraction& operator+=(const Fraction&);
    Fraction& operator-=(const Fraction&);
    Fraction& operator*=(const Fraction&);
    Fraction& operator/=(const Fraction&);
};

/*!
 * \brief Construct from a numerator and a denominator
 *
 * The fraction is automatically canonicalized, i.e. reduced to the irreducible
 * form.
 */
template<typename Unsigned>
Fraction<Unsigned>::Fraction(const Unsigned& numerator, const Unsigned& denominator)
  : _num(numerator),
    _den(denominator)
{
  if (Unsigned divisor = gcd(numerator, denominator)) {
    _num /= divisor;
    _den = denominator / divisor;
  }
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
  stream << _num << '/' << _den.value();
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
  Fraction c(_den, other._den);

  if (_den *= c._den) {
    Unsigned a = _num * c._den.value();
    Unsigned b = other._num * c._num;

    _den.validate(a / c._den.value() == _num);

    _num = a + b;

    _den.validate(_num >= a && (!c._num || b / c._num == other._num));
  }
  else {
    _num = 0;
  }

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator-=(const Fraction& other)
{
  Fraction c(_den, other._den);
  Unsigned a = _num * c._den.value();
  Unsigned b = other._num * c._num;

  _den *= c._den;
  _den.validate(a >= b && a / c._den.value() == _num && (!c._num || b / c._num == other._num));

  _num = (a - b) * bool(_den);

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(const Fraction& other)
{
  Fraction a(_num, other._den);
  Fraction b(other._num, _den);

  _den = a._den * b._den;
  _num = a._num * b._num * bool(_den);

  _den.validate(!b._num || _num / b._num == a._num);

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator/=(const Fraction& other)
{
  return *this *= other.inverse();
}

} // namespace Chic

#endif // CHIC_FRACTION_HPP
