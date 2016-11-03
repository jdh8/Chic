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

#ifndef CHIC_STEP_HPP
#define CHIC_STEP_HPP

#include <iostream>
#include <cstdlib>

namespace Chic {

template<typename Key>
class Step
{
  private:
    Key _first;
    Key _second;
    int _symbol;

  public:
    Step(Key = {}, int = 0);
    Step(Key, Key, int);

    Key first() const;
    Key second() const;
    int symbol() const;
    operator bool() const;
};

template<typename Key>
Step<Key>::Step(Key first, int symbol)
  : _first(first),
    _second(0),
    _symbol(symbol)
{}

template<typename Key>
Step<Key>::Step(Key first, Key second, int symbol)
  : _first(first),
    _second(second),
    _symbol(symbol)
{}

template<typename Key>
Key Step<Key>::first() const
{
  return _first;
}

template<typename Key>
Key Step<Key>::second() const
{
  return _second;
}

template<typename Key>
int Step<Key>::symbol() const
{
  return _symbol;
}

template<typename Key>
Step<Key>::operator bool() const
{
  return !!_first;
}

template<typename Key>
std::ostream& operator<<(std::ostream& stream, Step<Key> step)
{
  Key first = step.first();
  Key second = step.second();
  int symbol = step.symbol();

  if (second) {
    int shift = std::abs(symbol) - 1;

    if (shift < ' ') {
      for (int k = 0; k < shift; ++k)
        stream << "√";
      return stream << first << (symbol < 0 ? "^-" : "^") << second;
    }

    switch (symbol) {
      case '!':
        return stream << first << "! / " << second << '!';
      case '!' + 1:
        return stream << '(' << first << "! + " << second << "!) / " << second << '!';
      case '!' - 1:
        return stream << '(' << first << "! - " << second << "!) / " << second << '!';
      default:
        return stream << first << ' ' << char(symbol) << ' ' << second;
    }
  }
  else switch (symbol) {
    case '!':
      return stream << first << '!';
    case 's':
      return stream << "√" << first;
    default:
      return stream << char(symbol) << first;
  }
}

} // namespace Chic

#endif // CHIC_STEP_HPP
