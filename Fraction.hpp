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
 * If the result is not representable, nan is returned.
 *
 * Unsigned integer overflow wraps around, but signed integer overflow causes
 * undefined behavior.  Therefore, the underlying type must be unsigned.
 */
template<typename Unsigned>
class Fraction : public Arithmetic<Fraction<Unsigned>>
{
  private:
    Unsigned _num;
    Integer<Unsigned> _den;

    Fraction& apply(const Fraction&);

  public:
    Fraction() = default;
    Fraction(const Unsigned&);
    Fraction(const Unsigned&, const Unsigned&);
    Fraction(const Integer<Unsigned>&);
    Fraction(std::size_t, int);

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
    Fraction sqrt() const;

    Fraction factorial() const;
    Fraction factorial(const Fraction&) const;

    Fraction pow(Unsigned) const;
    Fraction pow(const Fraction&) const;

    explicit operator bool() const { return _num && _den; }

    Fraction& operator++() { _num += _den.value(); return *this; }
    Fraction& operator--() { _num -= _den.value(); return *this; }

    Fraction& operator+=(const Fraction&);
    Fraction& operator-=(const Fraction&);
    Fraction& operator*=(const Fraction&);
    Fraction& operator/=(const Fraction&);
};

template<typename Unsigned>
Fraction<Unsigned>::Fraction(const Unsigned& value)
  : _num(value),
    _den(1)
{}

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
Fraction<Unsigned>::Fraction(const Integer<Unsigned>& integer)
  : Fraction(integer.value())
{}

template<typename Unsigned>
Fraction<Unsigned>::Fraction(std::size_t repeat, int digit)
  : _num(Integer<Unsigned>(repeat, digit).value()),
    _den(1)
{}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::apply(const Fraction& other)
{
  _den *= other._den;

  Unsigned cache = _num * other._num * !!_den;

  _den *= !_num || cache / _num == other._num;
  _num = cache;

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::inverse() const
{
  Fraction result;

  result._num = _den.value();
  result._den = _num * !!_den;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::sqrt() const
{
  Fraction result;

  result._num = std::sqrt(_num);
  result._den = _den.sqrt() * (result._num * result._num == _num);
  result._num *= !!result._den;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::factorial() const
{
  Fraction result = Integer<Unsigned>(_num).factorial();

  result._den *= !!result._num;

  if (!result._num)
    result._num = _den.value() == 1;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::factorial(const Fraction& lesser) const
{
  Fraction result = 1;

  if (_den == lesser._den && _num >= lesser._num && _den && !((_num - lesser._num) % _den))
    for (Fraction multiplier = *this; _num > lesser._num; --multiplier)
      result.apply(multiplier);
  else
    result._den = result._num = 0;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::pow(Unsigned exponent) const
{
  Fraction base = *this;
  Fraction result = 1;

  for (; exponent; exponent >>= 1) {
    if (exponent & 1)
      result.apply(base);
    base.apply(base);
  }
  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::pow(const Fraction& exponent) const
{
  return exponent.denominator().value() == 1 ? pow(exponent.numerator()) : Fraction(0, 0);
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator+=(const Fraction& other)
{
  Fraction c(_den.value(), other._den.value());

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
  Fraction c(_den.value(), other._den.value());

  if (_den *= c._den) {
    Unsigned a = _num * c._den.value();
    Unsigned b = other._num * c._num;

    _den *= a >= b && a / c._den.value() == _num && (!c._num || b / c._num == other._num);
    _num = (a - b) * !!_den;
  }
  else {
    _num = 0;
  }

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(const Fraction& other)
{
  Fraction a(_num, other._den.value());
  Fraction b(other._num, _den.value());

  return *this = a.apply(b);
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator/=(const Fraction& other)
{
  return *this *= other.inverse();
}

template<typename Unsigned>
bool operator==(const Fraction<Unsigned>& x, const Fraction<Unsigned>& y)
{
  return x.denominator() && x.denominator() == y.denominator() && x.numerator() == y.numerator();
}

template<typename Character, typename Unsigned>
std::basic_ostream<Character>& operator<<(std::basic_ostream<Character>& stream, const Fraction<Unsigned>& fraction)
{
  if (fraction.denominator()) {
    stream << fraction.numerator();

    if (fraction.denominator().value() != 1)
      stream << '/' << fraction.denominator().value();
  }
  else {
    stream << (fraction.numerator() ? "inf" : "nan");
  }

  return stream;
}

} // namespace Chic

namespace std {

template<typename Unsigned>
struct hash<Chic::Fraction<Unsigned>>
{
  std::size_t operator()(const Chic::Fraction<Unsigned>& fraction) const
  {
    Chic::Integer<std::size_t> numerator = fraction.numerator();

    return Chic::rotate(numerator, (std::numeric_limits<std::size_t>::digits / 2)).value() ^ fraction.denominator().value();
  }
};

} // namespace std

#endif // CHIC_FRACTION_HPP
