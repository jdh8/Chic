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

#ifndef CHIC_EXPRESSION_HPP
#define CHIC_EXPRESSION_HPP

#include "Integer.hpp"

namespace Chic {

template<typename Unsigned>
class Expression
{
  private:
    Integer<Unsigned> _first;
    Integer<Unsigned> _second;
    char _symbol;

  public:
    static const char sqrt = 2;

    Expression(const Integer<Unsigned>& = 0, char = 0);
    Expression(const Integer<Unsigned>&, const Integer<Unsigned>&, char);

    const Integer<Unsigned>& first() const;
    const Integer<Unsigned>& second() const;
    char symbol() const;
    operator bool() const;
};

template<typename Unsigned>
Expression<Unsigned>::Expression(const Integer<Unsigned>& first, char symbol)
  : _first(first),
    _symbol(symbol)
{}

template<typename Unsigned>
Expression<Unsigned>::Expression(const Integer<Unsigned>& first, const Integer<Unsigned>& second, char symbol)
  : _first(first),
    _second(second),
    _symbol(symbol)
{}

template<typename Unsigned>
const Integer<Unsigned>& Expression<Unsigned>::first() const
{
  return _first;
}

template<typename Unsigned>
const Integer<Unsigned>& Expression<Unsigned>::second() const
{
  return _second;
}

template<typename Unsigned>
char Expression<Unsigned>::symbol() const
{
  return _symbol;
}

template<typename Unsigned>
Expression<Unsigned>::operator bool() const
{
  return _first;
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
