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

#include <iosfwd>

namespace Chic {

template<typename Index>
class Expression
{
  private:
    Index _first;
    Index _second;
    signed char _symbol;

  public:
    Expression(const Index& = {}, signed char = 0);
    Expression(const Index&, const Index&, signed char);

    const Index& first() const;
    const Index& second() const;
    signed char symbol() const;
    operator bool() const;
};

template<typename Index>
Expression<Index>::Expression(const Index& first, signed char symbol)
  : _first(first),
    _symbol(symbol)
{}

template<typename Index>
Expression<Index>::Expression(const Index& first, const Index& second, signed char symbol)
  : _first(first),
    _second(second),
    _symbol(symbol)
{}

template<typename Index>
const Index& Expression<Index>::first() const
{
  return _first;
}

template<typename Index>
const Index& Expression<Index>::second() const
{
  return _second;
}

template<typename Index>
signed char Expression<Index>::symbol() const
{
  return _symbol;
}

template<typename Index>
Expression<Index>::operator bool() const
{
  return !!_first;
}

template<typename Character, typename Index>
std::basic_ostream<Character>& operator<<(std::basic_ostream<Character>& stream, const Expression<Index>& expr)
{
  if (expr.second()) switch (expr.symbol()) {
    case -'^':
      return stream << expr.first() << " ^-" << expr.second();
    default:
      return stream << expr.first() << ' ' << expr.symbol() << ' ' << expr.second();
  }
  else switch (expr.symbol()) {
    case 's':
      return stream << "√" << expr.first();
    case '!':
      return stream << expr.first() << '!';
    default:
      return stream << expr.symbol() << expr.first();
  }
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
