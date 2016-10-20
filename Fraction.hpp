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
    const Unsigned& numerator() const { return _num; }

    /*!
     * \brief Denominator of the fraction
     *
     * Integer<Unsigned> mixes zero with invalid representation.  This makes it
     * a perfect denominator because zero denominator is also invalid.
     */
    const Integer<Unsigned>& denominator() const { return _den; }

    Fraction inverse() const;

    template<typename Character>
    explicit operator std::basic_string<Character>() const;

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
  if (Unsigned divisor = detail::gcd(numerator, denominator)) {
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
Fraction<Unsigned>::operator std::basic_string<Character>() const
{
  std::basic_ostringstream<Character> stream;
  stream << _num << '/' << _den.value();
  return stream.str();
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator+=(const Fraction& other)
{
  Fraction c(_den, other._den);

  if (_den *= c._den) {
    Unsigned a = _num * c._den.value();
    Unsigned b = other._num * c._num;

    _den *= a / c._den.value() == _num;
    _num = a + b;
    _den *= _num >= a && (!c._num || b / c._num == other._num);
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
  _den *= a >= b && a / c._den.value() == _num && (!c._num || b / c._num == other._num);
  _num = (a - b) * !!_den;

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(const Fraction& other)
{
  Fraction a(_num, other._den);
  Fraction b(other._num, _den);

  _den = a._den * b._den;
  _num = a._num * b._num * !!_den;
  _den *= !b._num || _num / b._num == a._num;

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator/=(const Fraction& other)
{
  return *this *= other.inverse();
}

template<typename Unsigned>
bool operator==(const Fraction<Unsigned>& x, const Fraction<Unsigned>& y)
{
  return x.denominator() && y.denominator() && x.numerator() == y.numerator() && x.denominator() == y.denominator();
}

} // namespace Chic

namespace std {

template<typename Unsigned>
bool isfinite(const Chic::Fraction<Unsigned>& fraction)
{
  return fraction.denominator();
}

template<typename Unsigned>
bool isinf(const Chic::Fraction<Unsigned>& fraction)
{
  return fraction.numerator() && !fraction.denominator();
}

template<typename Unsigned>
bool isnan(const Chic::Fraction<Unsigned>& fraction)
{
  return !(fraction.numerator() || fraction.denominator());
}

template<typename Unsigned>
struct hash<Chic::Fraction<Unsigned>>
{
  std::size_t operator()(const Chic::Fraction<Unsigned>& fraction) const
  {
    Chic::Integer<std::size_t> numerator = fraction.numerator();

    return Chic::rotate(numerator, (std::numeric_limits<std::size_t>::digits / 2)) ^ fraction.denominator();
  }
};

} // namespace std

#endif // CHIC_FRACTION_HPP
