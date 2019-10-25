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

#include "Entry.hpp"
#include <iosfwd>

namespace Chic {

template<typename Unsigned>
class Fraction : public Arithmetic<Fraction<Unsigned>>
{
  private:
    Overflow<Unsigned> _num;
    Overflow<Unsigned> _den;

    Fraction& operator++();
    Fraction& operator--();
    Fraction& _apply(Fraction);

  public:
    enum class Canonical_t {};
    static const Canonical_t Canonical = {};

    static Fraction inf();
    static Fraction nan();

    Fraction() = default;
    Fraction(Unsigned);
    Fraction(Unsigned, Unsigned);
    Fraction(Canonical_t, Unsigned, Unsigned);
    Fraction(Concatenate_t, std::size_t, int);

    Unsigned num() const;
    Unsigned den() const;

    explicit operator bool() const;

    Fraction inverse() const;
    Fraction sqrt() const;
    Fraction square() const;

    Fraction factorial() const;
    Fraction factorial(Fraction) const;

    Fraction pow(Unsigned) const;
    Fraction pow(Fraction) const;

    Fraction& operator+=(Fraction);
    Fraction& operator-=(Fraction);
    Fraction& operator*=(Fraction);
    Fraction& operator/=(Fraction);
};

template<typename Unsigned>
Fraction<Unsigned>::Fraction(Unsigned value)
  : _num(value),
    _den(1)
{}

template<typename Unsigned>
Fraction<Unsigned>::Fraction(Unsigned num, Unsigned den)
  : _num(num),
    _den(den)
{
  if (Unsigned divisor = gcd(num, den)) {
    _num = num / divisor;
    _den = den / divisor;
  }
}

template<typename Unsigned>
Fraction<Unsigned>::Fraction(Canonical_t, Unsigned num, Unsigned den)
  : _num(num),
    _den(den)
{}

template<typename Unsigned>
Fraction<Unsigned>::Fraction(Concatenate_t, std::size_t repeats, int digit)
  : _num(concatenate<Unsigned>(repeats, digit)),
    _den(1)
{}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::inf()
{
  return { Canonical, 1, 0 };
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::nan()
{
  return { Canonical, 0, 0 };
}

template<typename Unsigned>
Unsigned Fraction<Unsigned>::num() const
{
  return _num;
}

template<typename Unsigned>
Unsigned Fraction<Unsigned>::den() const
{
  return _den;
}

template<typename Unsigned>
Fraction<Unsigned>::operator bool() const
{
  return num() || !den();
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator++()
{
  _num = num() + den();
  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator--()
{
  _num = num() - den();
  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::_apply(Fraction other)
{
  bool overflow = _num *= other.num();
  bool invalid = _den *= other.den();

  _den *= !(invalid || overflow);
  _num = _num | (overflow && !(invalid || _num));

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::inverse() const
{
  return { Canonical, den(), num() * !!den() };
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::sqrt() const
{
  Fraction result(Canonical, std::sqrt(num()), std::sqrt(den()));
  bool valid = (result._num * result._num == num()) && (result._den * result._den == den());

  result._num *= valid || !den();
  result._den *= valid;

  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::square() const
{
  return Fraction(*this)._apply(*this);
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::factorial() const
{
  if (den() == 1) {
    if (Unsigned essay = Chic::factorial(_num))
      return essay;

    return inf();
  }
  return nan();
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::factorial(Fraction other) const
{
  if (den() == 1 && other.den() == 1) {
    if (Unsigned integer = Entry<Unsigned>((std::max)(num(), other.num())).factorial((std::min)(num(), other.num()))) {
      Fraction fraction(integer);
      return num() > other.num() ? fraction : fraction.inverse();
    }
  }
  return nan();
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::pow(Unsigned exponent) const
{
  Fraction base = *this;
  Fraction result = 1;

  for (; exponent; exponent >>= 1) {
    if (exponent & 1)
      result._apply(base);
    base = base.square();
  }
  return result;
}

template<typename Unsigned>
Fraction<Unsigned> Fraction<Unsigned>::pow(Fraction exponent) const
{
  if (exponent.den() == 1)
    return pow(exponent.num());

  return nan();
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator+=(Fraction other)
{
  Fraction fraction(_den, other.den());

  if (_den *= fraction.den()) {
    _num = 0;
  }
  else {
    bool overflow = (_num *= fraction.den()) || (fraction._num *= other.num()) || (_num += fraction.num());

    _den *= !overflow;
    _num = _num | (overflow && !_num);
  }

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator-=(Fraction other)
{
  Fraction fraction(_den, other.den());

  bool invalid = (_den *= fraction.den()) || (_num *= fraction.den()) || (fraction._num *= other.num()) || (_num -= fraction.num());

  _num *= !invalid;
  _den *= !invalid;

  return *this;
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator*=(Fraction other)
{
  Fraction a(num(), other.den());
  Fraction b(other.num(), den());

  return *this = a._apply(b);
}

template<typename Unsigned>
Fraction<Unsigned>& Fraction<Unsigned>::operator/=(Fraction other)
{
  return *this *= other.inverse();
}

template<typename Unsigned>
bool operator==(Fraction<Unsigned> x, Fraction<Unsigned> y)
{
  return x.den() && x.den() == y.den() && x.num() == y.num();
}

template<typename Character, typename Unsigned>
std::basic_ostream<Character>& operator<<(std::basic_ostream<Character>& stream, Fraction<Unsigned> fraction)
{
  if (fraction.den() == 1)
    return stream << fraction.num();

  if (fraction.den())
    return stream << '(' << fraction.num() << '/' << fraction.den() << ')';

  if (fraction.num())
    return stream << 'i' << 'n' << 'f';

  return stream << 'n' << 'a' << 'n';
}

} // namespace Chic

namespace std {

template<typename Unsigned>
bool isfinite(Chic::Fraction<Unsigned> fraction)
{
  return fraction.den();
}

template<typename Unsigned>
bool isinf(Chic::Fraction<Unsigned> fraction)
{
  return fraction.num() && !fraction.den();
}

template<typename Unsigned>
bool isnan(Chic::Fraction<Unsigned> fraction)
{
  return !(fraction.num() || fraction.den());
}

template<typename Unsigned>
bool isnormal(Chic::Fraction<Unsigned> fraction)
{
  return fraction.num() && fraction.den();
}

template<typename Unsigned>
struct hash<Chic::Fraction<Unsigned>>
{
  std::size_t operator()(Chic::Fraction<Unsigned> fraction) const
  {
    std::size_t num = fraction.num();

    return Chic::rotate(num, (std::numeric_limits<std::size_t>::digits / 2)) ^ fraction.den();
  }
};

} // namespace std

#endif // CHIC_FRACTION_HPP
