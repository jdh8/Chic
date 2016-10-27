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

#ifndef CHIC_OVERFLOW_HPP
#define CHIC_OVERFLOW_HPP

namespace Chic {

template<typename Unsigned>
class Overflow
{
  private:
    Unsigned _value;

  public:
    Overflow() = default;
    Overflow(Unsigned);

    operator Unsigned() const;

    bool operator+=(Unsigned);
    bool operator-=(Unsigned);
    bool operator*=(Unsigned);
};

template<typename Unsigned>
Overflow<Unsigned>::Overflow(Unsigned value)
  : _value(value)
{}

template<typename Unsigned>
Overflow<Unsigned>::operator Unsigned() const
{
  return _value;
}

template<typename Unsigned>
bool Overflow<Unsigned>::operator+=(Unsigned other)
{
  #ifdef __GNUC__
    return __builtin_add_overflow(_value, other, &_value);
  #else
    _value += other;
    return _value < other;
  #endif
}

template<typename Unsigned>
bool Overflow<Unsigned>::operator-=(Unsigned other)
{
  #ifdef __GNUC__
    return __builtin_sub_overflow(_value, other, &_value);
  #else
    bool overflow = _value < other;
    _value -= other;
    return overflow;
  #endif
}

template<typename Unsigned>
bool Overflow<Unsigned>::operator*=(Unsigned other)
{
  #ifdef __GNUC__
    return __builtin_mul_overflow(_value, other, &_value);
  #else
    Unsigned cache = _value;
    _value *= other;
    return other && _value / other != cache;
  #endif
}

} // namespace Chic

#endif // CHIC_OVERFLOW_HPP
