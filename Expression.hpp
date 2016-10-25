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
#include <cmath>

namespace Chic {

template<typename Index>
class Expression
{
  private:
    Index _first;
    Index _second;
    int _symbol;

  public:
    Expression(const Index& = {}, int = 0);
    Expression(const Index&, const Index&, int);

    const Index& first() const;
    const Index& second() const;
    int symbol() const;
    operator bool() const;
};

template<typename Index>
Expression<Index>::Expression(const Index& first, int symbol)
  : _first(first),
    _symbol(symbol)
{}

template<typename Index>
Expression<Index>::Expression(const Index& first, const Index& second, int symbol)
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
int Expression<Index>::symbol() const
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
  if (expr.second()) {
    int shift = std::abs(expr.symbol()) - 1;

    if (shift < ' ') {
      for (int k = 0; k < shift; ++k)
        stream << "√";
      return stream << expr.first() << (expr.symbol() < 0 ? "^-" : "^") << expr.second();
    }

    return stream << expr.first() << ' ' << char(expr.symbol()) << ' ' << expr.second();
  }
  else switch (expr.symbol()) {
    case '!':
      return stream << expr.first() << '!';
    case 's':
      return stream << "√" << expr.first();
    default:
      return stream << char(expr.symbol()) << expr.first();
  }
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
