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

#ifndef CHIC_CHECKED_HPP
#define CHIC_CHECKED_HPP

namespace Chic {
/*!
 * \brief Cross-platform integer overflow checker
 */
template<typename Unsigned>
class Checked
{
  private:
    Unsigned _value;

  public:
    Checked();
    Checked(const Unsigned&);

    operator const Unsigned&() const { return _value; }

    bool operator+=(const Unsigned&);
    bool operator-=(const Unsigned&);
    bool operator*=(const Unsigned&);
};

template<typename Unsigned>
Checked<Unsigned>::Checked()
{}

template<typename Unsigned>
Checked<Unsigned>::Checked(const Unsigned& value)
  : _value(value)
{}

template<typename Unsigned>
bool Checked<Unsigned>::operator+=(const Unsigned& other)
{
  #ifdef __GNUC__
    return !__builtin_add_overflow(_value, other, &_value);
  #else
    _value += other;
    return _value < other;
  #endif
}

template<typename Unsigned>
bool Checked<Unsigned>::operator-=(const Unsigned& other)
{
  #ifdef __GNUC__
    return !__builtin_sub_overflow(_value, other, &_value);
  #else
    bool sane = _value >= other;
    _value -= other;
    return sane;
  #endif
}

template<typename Unsigned>
bool Checked<Unsigned>::operator*=(const Unsigned& other)
{
  #ifdef __GNUC__
    return !__builtin_mul_overflow(_value, other, &_value);
  #else
    Unsigned cache = _value;
    _value *= other;
    return !other || _value / other == cache;
  #endif
}

} // namespace Chic

#endif // CHIC_CHECKED_HPP
