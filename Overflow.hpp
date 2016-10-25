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

#include <limits>
#include <type_traits>

namespace Chic {

template<typename Integer>
class OverflowBase
{
  protected:
    OverflowBase();
};

template<typename Integer>
OverflowBase<Integer>::OverflowBase()
{
  static_assert(std::numeric_limits<Integer>::is_integer, "The underlying type must be an integer.");
}

template<typename Integer, bool IsSigned = std::numeric_limits<Integer>::is_signed>
class Overflow;

template<typename Unsigned>
class Overflow<Unsigned, false>
{
  private:
    Unsigned _value;

  public:
    Overflow() = default;
    Overflow(Unsigned);

    template<typename Signed>
    Overflow(Overflow<Signed, true>);

    operator Unsigned() const;

    bool operator+=(Unsigned);
    bool operator-=(Unsigned);
    bool operator*=(Unsigned);
    void operator*=(bool);
};

template<typename Unsigned>
Overflow<Unsigned, false>::Overflow(Unsigned value)
  : _value(value)
{}

template<typename Unsigned>
template<typename Signed>
Overflow<Unsigned, false>::Overflow(Overflow<Signed, true> other)
  : _value(other)
{}

template<typename Unsigned>
Overflow<Unsigned, false>::operator Unsigned() const
{
  return _value;
}

template<typename Unsigned>
bool Overflow<Unsigned, false>::operator+=(Unsigned other)
{
  #ifdef __GNUC__
    return __builtin_add_overflow(_value, other, &_value);
  #else
    _value += other;
    return _value < other;
  #endif
}

template<typename Unsigned>
bool Overflow<Unsigned, false>::operator-=(Unsigned other)
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
bool Overflow<Unsigned, false>::operator*=(Unsigned other)
{
  #ifdef __GNUC__
    return __builtin_mul_overflow(_value, other, &_value);
  #else
    Unsigned cache = _value;
    _value *= other;
    return other && _value / other != cache;
  #endif
}

template<typename Unsigned>
void Overflow<Unsigned, false>::operator*=(bool condition)
{
  _value *= condition;
}

template<typename Signed>
class Overflow<Signed, true>
{
  private:
    typedef typename std::make_unsigned<Signed>::type Unsigned_t;

    Signed _value;

    static Signed _convert(Unsigned_t);

  public:
    Overflow() = default;
    Overflow(Signed);

    template<typename Unsigned>
    Overflow(Overflow<Unsigned, false>);

    operator Signed() const;

    template<typename Other>
    bool operator+=(Other);

    template<typename Other>
    bool operator-=(Other);

    template<typename Other>
    bool operator*=(Other);

    void operator*=(bool);
};

template<typename Signed>
Signed Overflow<Signed, true>::_convert(Unsigned_t x)
{
  const Signed min = std::numeric_limits<Signed>::min();

  return x >= min ? Signed(x - min) + min : x;
}

template<typename Signed>
Overflow<Signed, true>::Overflow(Signed value)
  : _value(value)
{}

template<typename Signed>
template<typename Unsigned>
Overflow<Signed, true>::Overflow(Overflow<Unsigned, false> other)
  : _value(_convert(other))
{}

template<typename Signed>
Overflow<Signed, true>::operator Signed() const
{
  return _value;
}

template<typename Signed>
template<typename Other>
bool Overflow<Signed, true>::operator+=(Other other)
{
  #ifdef __GNUC__
    return __builtin_add_overflow(_value, other, &_value);
  #else
    Overflow<Unsigned_t> copy(*this);
    bool overflow = copy += other;

    *this = copy;
    return overflow;
  #endif
}

template<typename Signed>
template<typename Other>
bool Overflow<Signed, true>::operator-=(Other other)
{
  #ifdef __GNUC__
    return __builtin_sub_overflow(_value, other, &_value);
  #else
    Overflow<Unsigned_t> copy(*this);
    bool overflow = copy -= other;

    *this = copy;
    return overflow;
  #endif
}

template<typename Signed>
template<typename Other>
bool Overflow<Signed, true>::operator*=(Other other)
{
  #ifdef __GNUC__
    return __builtin_mul_overflow(_value, other, &_value);
  #else
    Overflow<Unsigned_t> copy(*this);
    bool overflow = copy *= other;

    *this = copy;
    return overflow;
  #endif
}

template<typename Signed>
void Overflow<Signed, true>::operator*=(bool condition)
{
  _value *= condition;
}

} // namespace Chic

#endif // CHIC_OVERFLOW_HPP
