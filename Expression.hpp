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
#include <cstdlib>

namespace Chic {

template<typename Key>
class Expression
{
  private:
    Key _first;
    Key _second;
    int _symbol;

  public:
    Expression(Key = {}, int = 0);
    Expression(Key, Key, int);

    Key first() const;
    Key second() const;
    int symbol() const;
    operator bool() const;
};

template<typename Key>
Expression<Key>::Expression(Key first, int symbol)
  : _first(first),
    _symbol(symbol)
{}

template<typename Key>
Expression<Key>::Expression(Key first, Key second, int symbol)
  : _first(first),
    _second(second),
    _symbol(symbol)
{}

template<typename Key>
Key Expression<Key>::first() const
{
  return _first;
}

template<typename Key>
Key Expression<Key>::second() const
{
  return _second;
}

template<typename Key>
int Expression<Key>::symbol() const
{
  return _symbol;
}

template<typename Key>
Expression<Key>::operator bool() const
{
  return !!_first;
}

template<typename Character, typename Key>
std::basic_ostream<Character>& operator<<(std::basic_ostream<Character>& stream, Expression<Key> expression)
{
  if (expression.second()) {
    int shift = std::abs(expression.symbol()) - 1;

    if (shift < ' ') {
      for (int k = 0; k < shift; ++k)
        stream << "√";
      return stream << expression.first() << (expression.symbol() < 0 ? "^-" : "^") << expression.second();
    }

    return stream << expression.first() << ' ' << char(expression.symbol()) << ' ' << expression.second();
  }
  else switch (expression.symbol()) {
    case '!':
      return stream << expression.first() << '!';
    case 's':
      return stream << "√" << expression.first();
    default:
      return stream << char(expression.symbol()) << expression.first();
  }
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
