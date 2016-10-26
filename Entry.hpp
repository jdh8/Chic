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

#ifndef CHIC_ENTRY_HPP
#define CHIC_ENTRY_HPP

#include "Arithmetic.hpp"
#include "Overflow.hpp"
#include "Factorial.hpp"
#include <cmath>

namespace Chic {

template<typename Unsigned>
class Entry : public Arithmetic<Entry<Unsigned>>
{
  private:
    Overflow<Unsigned> _value;

  public:
    Entry(Unsigned = 0);
    Entry(std::size_t, int);

    operator Unsigned() const;
    Unsigned value() const;

    Entry& operator*=(bool);

    Entry& operator+=(Entry);
    Entry& operator-=(Entry);
    Entry& operator*=(Entry);
    Entry& operator/=(Entry);

    Entry pow(Unsigned) const;
    Entry sqrt() const;
    Entry factorial() const;
    Entry factorial(Entry) const;
};

template<typename Unsigned>
Entry<Unsigned>::Entry(Unsigned value)
  : _value(value)
{}

template<typename Unsigned>
Entry<Unsigned>::Entry(std::size_t repeats, int digit)
  : _value(0)
{
  while (repeats--)
    _value = 10 * _value + digit;
}

template<typename Unsigned>
Entry<Unsigned>::operator Unsigned() const
{
  return _value;
}

template<typename Unsigned>
Unsigned Entry<Unsigned>::value() const
{
  return _value;
}

template<typename Unsigned>
Entry<Unsigned>& Entry<Unsigned>::operator*=(bool condition)
{
  _value *= condition;
  return *this;
}

template<typename Unsigned>
Entry<Unsigned>& Entry<Unsigned>::operator+=(Entry other)
{
  bool overflow = _value += other.value();
  _value *= !overflow;
  return *this;
}

template<typename Unsigned>
Entry<Unsigned>& Entry<Unsigned>::operator-=(Entry other)
{
  bool underflow = _value -= other.value();
  _value *= !underflow;
  return *this;
}

template<typename Unsigned>
Entry<Unsigned>& Entry<Unsigned>::operator*=(Entry other)
{
  bool overflow = _value *= other.value();
  _value *= !overflow;
  return *this;
}

template<typename Unsigned>
Entry<Unsigned>& Entry<Unsigned>::operator/=(Entry other)
{
  Unsigned result = _value / other._value;
  _value = result * (other._value * result == _value);
  return *this;
}

template<typename Unsigned, typename Other>
Entry<Unsigned> operator*(Entry<Unsigned> x, Other other)
{
  return x *= other;
}

template<typename Unsigned>
Entry<Unsigned> operator*(bool condition, Entry<Unsigned> x)
{
  return x *= condition;
}

template<typename Unsigned>
Entry<Unsigned> Entry<Unsigned>::pow(Unsigned exponent) const
{
  Entry base = *this;
  Entry result = 1;

  for (; exponent; exponent >>= 1) {
    if (exponent & 1)
      result *= base;
    base *= base;
  }
  return result;
}

template<typename Unsigned>
Entry<Unsigned> Entry<Unsigned>::sqrt() const
{
  Unsigned result = std::sqrt(value());
  return (result * result == value()) * result;
}

template<typename Unsigned>
Entry<Unsigned> Entry<Unsigned>::factorial() const
{
  return Chic::factorial(_value);
}

template<typename Unsigned>
Entry<Unsigned> Entry<Unsigned>::factorial(Entry lesser) const
{
  Entry result = *this >= lesser;

  for (Entry multiplier = _value; multiplier > lesser; --multiplier)
    result *= multiplier;

  return result;
}

} // namespace Chic

namespace std {

template<typename Unsigned>
struct hash<Chic::Entry<Unsigned>> : hash<Unsigned>
{};

} // namespace std

#endif // CHIC_ENTRY_HPP
