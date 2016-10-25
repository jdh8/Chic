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
    Overflow<Unsigned> _num;
    Overflow<Unsigned> _den;

  public:
    Fraction();
    Fraction(const Unsigned&);
    Fraction(const Unsigned&, const Unsigned&);
    Fraction(std::size_t, int);

    Unsigned numerator() const { return _num; }
    Unsigned denominator() const { return _den; }

    Fraction& apply(const Fraction&);

    Fraction inverse() const;
    Fraction sqrt() const;

    Fraction factorial() const;
    Fraction factorial(const Fraction&) const;

    Fraction pow(Unsigned) const;
    Fraction pow(const Fraction&) const;

    explicit operator bool() const { return _num && _den; }

    Fraction& operator++() { _num += _den; return *this; }
    Fraction& operator--() { _num -= _den; return *this; }

    Fraction& operator+=(const Fraction&);
    Fraction& operator-=(const Fraction&);
    Fraction& operator*=(const Fraction&);
    Fraction& operator/=(const Fraction&);
};

template<typename Unsigned>
Fraction<Unsigned>::Fraction()
  : _den(0)
{}

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
    _num = numerator / divisor;
    _den = denominator / divisor;
  }
}

template<typename Unsigned>
Fraction<Unsigned>::Fraction(std::size_t repeats, int digit)
  : _num(0),
    _den(1)
{
  while (repeats--)
    _num = 10 * _num + digit;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::apply(const Fraction& other)
{
  bool overflow = _num *= other._num;
  bool invalid = _den *= other._den;

  _den *= !(invalid || overflow);
  _num = _num | (overflow && !(invalid || _num));

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::inverse() const
{
  Fraction result;

  result._num = _den;
  result._den = _num * !!_den;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::sqrt() const
{
  Fraction result;

  result._num = std::sqrt(Unsigned(_num));
  result._den = std::sqrt(Unsigned(_den));
  
  bool valid = (result._num * result._num == _num) && (result._den * result._den == _den);

  result._num *= valid;
  result._den *= valid;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::factorial() const
{
  Fraction result;

  result._num = Integer<Unsigned>(_num).factorial().value();
  result._den = !!result._num;
  result._num = result._num | (_den == 1 && !result._num);

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
  return exponent._den == 1 ? pow(exponent._num) : Fraction(0, 0);
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator+=(const Fraction& other)
{
  Fraction fraction(_den, other._den);

  if (_den *= fraction._den) {
    _num = 0;
  }
  else {
    bool overflow = (_num *= fraction._den) || (fraction._num *= other._num) || (_num += fraction._num);

    _den *= !overflow;
    _num = _num | (overflow && !_num);
  }

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator-=(const Fraction& other)
{
  Fraction fraction(_den, other._den);

  bool invalid = (_den *= fraction._den) || (_num *= fraction._den) || (fraction._num *= other._num) || (_num -= fraction._num);

  _num *= !invalid;
  _den *= !invalid;

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(const Fraction& other)
{
  Fraction a(_num, other._den);
  Fraction b(other._num, _den);

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
  if (fraction.denominator() == 1)
    return stream << fraction.numerator();
  else if (fraction.denominator())
    return stream << '(' << fraction.numerator() << '/' << fraction.denominator() << ')';
  else
    return stream << (fraction.numerator() ? "inf" : "nan");
}

} // namespace Chic

namespace std {

template<typename Unsigned>
bool isfinite(const Chic::Fraction<Unsigned>& fraction)
{
  return fraction.denominator();
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
