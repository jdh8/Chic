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

namespace Chic {

class Expression
{
  private:
    std::string _expr;

  public:
    Expression(const char* = "");
    Expression(const std::string&);
    Expression(std::size_t, int);
    const std::string& operator()() const;
};

inline Expression::Expression(const char* expr)
  : _expr(expr)
{}

inline Expression::Expression(const std::string& expr)
  : _expr(expr)
{}

inline Expression::Expression(std::size_t repeats, int digit)
  : _expr(repeats, '0'|digit)
{}

inline const std::string& Expression::operator()() const
{
  return _expr;
}

inline Expression operator+(const Expression& x, const Expression& y)
{
  return "(" + x() + " + " + y() + ")";
}

inline Expression operator-(const Expression& x, const Expression& y)
{
  return "(" + x() + " - " + y() + ")";
}

inline Expression operator*(const Expression& x, const Expression& y)
{
  return "(" + x() + " * " + y() + ")";
}

inline Expression operator/(const Expression& x, const Expression& y)
{
  return "(" + x() + " / " + y() + ")";
}

inline Expression pow(const Expression& x, const Expression& y)
{
  return "(" + x() + " ^ " + y() + ")";
}

inline Expression sqrt(const Expression& x)
{
  return "√" + x();
}

inline Expression factorial(const Expression& x)
{
  return x() + "!";
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
