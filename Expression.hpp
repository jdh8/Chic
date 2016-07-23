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
    static std::string resolve(const Integer<Unsigned>&);

    Integer<Unsigned> _first;
    Integer<Unsigned> _second;
    char _operator;

  public:
    static const char sqrt = 2;

    Expression(const Expression&) = default;
    Expression(const Integer<Unsigned>& = 0, char = 0);
    Expression(const Integer<Unsigned>&, const Integer<Unsigned>&, char);

    operator bool() const;

    template<typename Graph>
    std::string resolve(const Graph&) const;
};

template<typename Unsigned>
Expression<Unsigned>::Expression(const Integer<Unsigned>& first, char operation)
  : _first(first),
    _operator(operation)
{}

template<typename Unsigned>
Expression<Unsigned>::Expression(const Integer<Unsigned>& first, const Integer<Unsigned>& second, char operation)
  : _first(first),
    _second(second),
    _operator(operation)
{}

template<typename Unsigned>
Expression<Unsigned>::operator bool() const
{
  return _first;
}

template<typename Unsigned>
template<typename Graph>
std::string Expression<Unsigned>::resolve(const Graph& graph) const
{
  switch (_operator) {
    case 0:
      return _first ? _first.str() : "";
    case sqrt:
      return "√" + graph[_first].resolve(graph);
    case '!':
      return graph[_first].resolve(graph) + '!';
  }
  return '(' + graph[_first].resolve(graph) + ' ' + _operator + ' ' + graph[_second].resolve(graph) + ')';
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
