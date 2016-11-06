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

#include "Annotation.hpp"
#include <ostream>
#include <cstdlib>

namespace Chic {

template<typename Key>
class Step
{
  private:
    Key _first;
    Key _second;
    Annotation<char> _note;

  public:
    Step(Key = {}, Annotation<char> = {});
    Step(Key, Key, Annotation<char>);

    Key first() const;
    Key second() const;
    Annotation<char> note() const;
    operator bool() const;
};

template<typename Key>
Step<Key>::Step(Key first, Annotation<char> note)
  : _first(first),
    _second(0),
    _note(note)
{}

template<typename Key>
Step<Key>::Step(Key first, Key second, Annotation<char> note)
  : _first(first),
    _second(second),
    _note(note)
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
Annotation<char> Step<Key>::note() const
{
  return _note;
}

template<typename Key>
Step<Key>::operator bool() const
{
  return !!_first;
}

namespace detail {

template<typename Key>
std::ostream& print_shifted_power(std::ostream& stream, signed char code, Key first, Key second)
{
  const char infix[][3] = { "^", "^-" };
  bool negative = code < 0;
  unsigned char cast = code;
  unsigned char shift = negative ? ~cast : cast;

  for (int iterations = 0; iterations < shift; ++iterations)
    stream << "√";

  return stream << first << infix[negative] << second;
}

template<typename Key>
std::ostream& print_factorial_quotient(std::ostream& stream, signed char code, Key first, Key second)
{
  switch (code) {
    case '\0':
      return stream << first << '!';
    case '/':
      return stream << first << "! / " << second << '!';
    case '+':
      return stream << '(' << first << "! + " << second << "!) / " << second << '!';
    case '-':
      return stream << '(' << first << "! + " << second << "!) / " << second << '!';
  }

  //TODO Find a better way to handle this error
  throw Annotation<char>('!', code);
}

} // namespace detail

template<typename Key>
std::ostream& operator<<(std::ostream& stream, Step<Key> step)
{
  switch (char base = step.note().base()) {
    case '^':
      return detail::print_shifted_power(stream, step.note().code(), step.first(), step.second());
    case '!':
      return detail::print_factorial_quotient(stream, step.note().code(), step.first(), step.second());
    case 's':
      return stream << "√" << step.first();
    default:
      return stream << step.first() << ' ' << base << ' ' << step.second();
  }
}

} // namespace Chic

#endif // CHIC_STEP_HPP
