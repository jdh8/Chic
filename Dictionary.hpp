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

template<typename Unsigned>
class Dictionary
{
  private:
    std::unordered_map<Integer<Unsigned>, Expression<Unsigned>> _graph;
    std::vector<std::vector<Integer<Unsigned>>> _hierarchy;
    const int _digit;

    void emplace(const Integer<Unsigned>&, const Expression<Unsigned>&);
    void binary(const Integer<Unsigned>&, const Integer<Unsigned>&);

    template<typename Function>
    void unary(const Function&, char, const Unsigned&);

  public:
    Dictionary(int);

    const Expression<Unsigned>& operator[](const Integer<Unsigned>&) const;
    const std::unordered_map<Integer<Unsigned>, Expression<Unsigned>>& graph() const;
    const std::vector<std::vector<Integer<Unsigned>>>& hierarchy() const;
    int digit() const;
    std::string resolve(const Expression<Unsigned>&) const;

    void grow();
    const Expression<Unsigned>& build(const Integer<Unsigned>&);
};

template<typename Unsigned>
Dictionary<Unsigned>::Dictionary(int digit)
  : _digit(digit)
{}

template<typename Unsigned>
const Expression<Unsigned>& Dictionary<Unsigned>::operator[](const Integer<Unsigned>& key) const
{
  static const Expression<Unsigned> empty;
  auto found = _graph.find(key);
  return found == _graph.end() ? empty : found->second;
}

template<typename Unsigned>
const std::unordered_map<Integer<Unsigned>, Expression<Unsigned>>& Dictionary<Unsigned>::graph() const
{
  return _graph;
}

template<typename Unsigned>
const std::vector<std::vector<Integer<Unsigned>>>& Dictionary<Unsigned>::hierarchy() const
{
  return _hierarchy;
}

template<typename Unsigned>
int Dictionary<Unsigned>::digit() const
{
  return _digit;
}

template<typename Unsigned>
std::string Dictionary<Unsigned>::resolve(const Expression<Unsigned>& expr) const
{
  const auto& first = expr.first();
  auto found = _graph.find(first);

  switch (expr.symbol()) {
    case 0:
      return first ? first.str() : "";
    case Expression<Unsigned>::sqrt:
      return "√" + resolve(found->second);
    case '!':
      return resolve(found->second) + '!';
  }
  return '(' + resolve(found->second) + ' ' + expr.symbol() + ' ' + resolve(_graph.find(expr.second())->second) + ')';
}

template<typename Unsigned>
void Dictionary<Unsigned>::emplace(const Integer<Unsigned>& key, const Expression<Unsigned>& expr)
{
  if (key && _graph.emplace(key, expr).second)
    _hierarchy.back().emplace_back(key);
}

template<typename Unsigned>
void Dictionary<Unsigned>::binary(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
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

template<typename Unsigned>
template<typename Function>
void Dictionary<Unsigned>::unary(const Function& function, char symbol, const Unsigned& thresh)
{
  auto& destination = _hierarchy.back();
  std::vector<Integer<Unsigned>> source;

  for (auto x: destination)
    for (auto y = function(x); y.value() > thresh; y = function(y))
      if (_graph.emplace(y, Expression<Unsigned>(x, symbol)).second)
        source.emplace_back(x = y);

  destination.insert(destination.end(), source.begin(), source.end());
}

template<typename Unsigned>
void Dictionary<Unsigned>::grow()
{
  _hierarchy.emplace_back();

  const auto* base = _hierarchy.data() - 1;
  auto size = _hierarchy.size();
  Integer<Unsigned> root(size, _digit);

  _graph[root] = root;
  _hierarchy.back().push_back(root);

  for (auto length = size / 2; length > 0; --length)
    for (const auto& x: base[length])
      for (const auto& y: base[size - length])
        binary(x, y);

  unary(sqrt<Unsigned>, Expression<Unsigned>::sqrt, 1);
  unary(factorial<Unsigned>, '!', 2);
}

template<typename Unsigned>
const Expression<Unsigned>& Dictionary<Unsigned>::build(const Integer<Unsigned>& key)
{
  while (true) {
    if (const auto& found = operator[](key))
      return found;
    grow();
  }
}

} // namespace Chic

#endif // CHIC_DICTIONARY_HPP
