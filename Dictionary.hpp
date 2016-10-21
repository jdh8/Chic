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

#ifndef CHIC_DICTIONARY_HPP
#define CHIC_DICTIONARY_HPP

#include "Expression.hpp"
#include <unordered_map>

namespace Chic {

template<typename Number>
class Dictionary
{
  private:
    std::unordered_map<Number, Expression<Number>> _graph;
    std::vector<std::vector<Number>> _hierarchy;
    const int _digit;

    void emplace(const Number&, const Expression<Number>&);
    void binary(const Number&, const Number&);

    template<typename Function>
    void unary(const Function&, char, const Number&);

  public:
    Dictionary(int);

    const Expression<Number>& operator[](const Number&) const;
    const std::unordered_map<Number, Expression<Number>>& graph() const;
    const std::vector<std::vector<Number>>& hierarchy() const;
    int digit() const;
    std::string resolve(const Expression<Number>&) const;

    void grow();
    const Expression<Number>& build(const Number&);
};

template<typename Number>
Dictionary<Number>::Dictionary(int digit)
  : _digit(digit)
{}

template<typename Number>
const Expression<Number>& Dictionary<Number>::operator[](const Number& key) const
{
  static const Expression<Number> empty;
  auto found = _graph.find(key);
  return found == _graph.end() ? empty : found->second;
}

template<typename Number>
const std::unordered_map<Number, Expression<Number>>& Dictionary<Number>::graph() const
{
  return _graph;
}

template<typename Number>
const std::vector<std::vector<Number>>& Dictionary<Number>::hierarchy() const
{
  return _hierarchy;
}

template<typename Number>
int Dictionary<Number>::digit() const
{
  return _digit;
}

template<typename Number>
std::string Dictionary<Number>::resolve(const Expression<Number>& expr) const
{
  const auto& first = expr.first();
  auto found = _graph.find(first);

  switch (expr.symbol()) {
    case 0:
      return first ? std::string(first) : "";
    case Expression<Number>::sqrt:
      return "√" + resolve(found->second);
    case '!':
      return resolve(found->second) + '!';
  }
  return '(' + resolve(found->second) + ' ' + expr.symbol() + ' ' + resolve(_graph.find(expr.second())->second) + ')';
}

template<typename Number>
void Dictionary<Number>::emplace(const Number& key, const Expression<Number>& expr)
{
  if (key && _graph.emplace(key, expr).second)
    _hierarchy.back().emplace_back(key);
}

template<typename Number>
void Dictionary<Number>::binary(const Number& x, const Number& y)
{
  emplace(x + y, { x, y, '+' });
  emplace(x * y, { x, y, '*' });
  emplace(pow(x, y), { x, y, '^' });
  emplace(pow(y, x), { y, x, '^' });

  emplace(x - y, { x, y, '-' });
  emplace(y - x, { y, x, '-' });
  emplace(x / y, { x, y, '/' });
  emplace(y / x, { y, x, '/' });
}

template<typename Number>
template<typename Function>
void Dictionary<Number>::unary(const Function& function, char symbol, const Number& thresh)
{
  auto& destination = _hierarchy.back();
  std::vector<Number> source;

  for (auto x: destination)
    for (auto y = function(x); y > thresh; y = function(y))
      if (_graph.emplace(y, Expression<Number>(x, symbol)).second)
        source.emplace_back(x = y);

  destination.insert(destination.end(), source.begin(), source.end());
}

template<typename Number>
void Dictionary<Number>::grow()
{
  typedef Number Function(const Number&);

  const Function* psqrt = sqrt;
  const Function* pfact = factorial;

  _hierarchy.emplace_back();

  const auto* base = _hierarchy.data() - 1;
  auto size = _hierarchy.size();
  Number root(size, _digit);

  _graph[root] = root;
  _hierarchy.back().push_back(root);

  for (auto length = size / 2; length > 0; --length)
    for (const auto& x: base[length])
      for (const auto& y: base[size - length])
        binary(x, y);

  unary(psqrt, Expression<Number>::sqrt, 1);
  unary(pfact, '!', 2);
}

template<typename Number>
const Expression<Number>& Dictionary<Number>::build(const Number& key)
{
  while (true) {
    if (const auto& found = operator[](key))
      return found;
    grow();
  }
}

} // namespace Chic

#endif // CHIC_DICTIONARY_HPP
