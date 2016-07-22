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

#include <string>
#include <cstring>

namespace Chic {

class Expression
{
  private:
    const Expression* _first;

    union {
      const Expression* _second;
      char _digit;
    };

    union {
      std::size_t _repeats;
      char _operator[4];
    };

  public:
    const std::size_t penalty;

    Expression();
    Expression(std::size_t, int);
    Expression(const char*, const Expression&);
    Expression(const char*, const Expression&, const Expression&);
    std::string str() const;
    operator bool() const;
};

inline Expression::Expression()
  : _first(NULL),
    _repeats(0),
    penalty(0)
{}

inline Expression::Expression(std::size_t repeats, int digit)
  : _first(NULL),
    _digit('0' | digit),
    _repeats(repeats),
    penalty(0)
{}

inline Expression::Expression(const char* string, const Expression& first)
  : _first(&first),
    _second(NULL),
    penalty(first.penalty + 1)
{
  std::strncpy(_operator, string, 4);
}

inline Expression::Expression(const char* string, const Expression& first, const Expression& second)
  : _first(&first),
    _second(&second),
    penalty(first.penalty + second.penalty + 3 + (string[1] == '/'))
{
  std::strncpy(_operator, string, 4);
}

inline std::string Expression::str() const
{
  if (_first)
    if (_second)
      return "(" + _first->str() + _operator + _second->str() + ")";
    else
      return *_operator == '!' ? _first->str() + _operator : _operator + _first->str();
  else
    return std::string(_repeats, _digit);
}

inline Expression::operator bool() const
{
  return _first || _repeats;
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
