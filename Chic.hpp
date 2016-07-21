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

#ifndef CHIC_HPP
#define CHIC_HPP

#include "Integer.hpp"
#include "Expression.hpp"
#include <unordered_map>
#include <memory>
#include <vector>

namespace Chic {

namespace internal {

template<typename Map, typename Unsigned>
void insert(Map& map, const Integer<Unsigned>& value, const char* infix, const Expression& x, const Expression& y)
{
  if (value) {
    std::unique_ptr<Expression>& reference = map[value];
    std::size_t penalty = x.penalty + y.penalty + 3 + (infix[1] == '/');

    if (!reference || penalty < reference->penalty)
      reference = std::unique_ptr<Expression>(new Expression(infix, x, y));
  }
}

template<typename Map, typename Pair>
void insert(Map& map, const Pair& x, const Pair& y)
{
  insert(map, x.first + y.first, " + ", *x.second, *y.second);
  insert(map, x.first * y.first, " * ", *x.second, *y.second);
  insert(map, pow(x.first, y.first), " ^ ", *x.second, *y.second);
  insert(map, pow(y.first, x.first), " ^ ", *y.second, *x.second);

  insert(map, x.first - y.first, " - ", *x.second, *y.second);
  insert(map, y.first - x.first, " - ", *y.second, *x.second);
  insert(map, x.first / y.first, " / ", *x.second, *y.second);
  insert(map, y.first / x.first, " / ", *y.second, *x.second);
}

template<typename Unsigned, typename Map>
void replicate_sqrt(Map& map)
{
  std::vector< std::pair<Integer<Unsigned>, Expression*> > copy;
  copy.reserve(map.size());

  for (const auto& pair: map)
    copy.emplace_back(pair.first, pair.second.get());

  for (const auto& pair: copy) {
    Integer<Unsigned> first = sqrt(pair.first);
    Expression* second = pair.second;

    while (first > 2) {
      std::unique_ptr<Expression>& reference = map[first];
      std::size_t penalty = second->penalty + 1;

      if (!reference || penalty < reference->penalty) {
        second = new Expression("√", *second);
        reference = std::unique_ptr<Expression>(second);
      }
      first = sqrt(first);
    }
  }
}

template<typename Unsigned, typename Map>
void replicate_factorial(Map& map)
{
  std::vector< std::pair<Integer<Unsigned>, Expression*> > copy;
  copy.reserve(map.size());

  for (const auto& pair: map)
    copy.emplace_back(pair.first, pair.second.get());

  for (const auto& pair: copy) {
    Integer<Unsigned> first = factorial(pair.first);
    Expression* second = pair.second;

    while (first > 2) {
      std::unique_ptr<Expression>& reference = map[first];
      std::size_t penalty = second->penalty + 1;

      if (!reference || penalty < reference->penalty) {
        second = new Expression("!", *second);
        reference = std::unique_ptr<Expression>(second);
      }
      first = factorial(first);
    }
  }
}

template<typename Map, typename Unsigned>
const Map& build(Map* memo, std::size_t size, int digit, const Unsigned& target)
{
  Map& map = memo[size];

  map.emplace(Integer<Unsigned>(size, digit), new Expression(size, digit));

  for (std::size_t length = size / 2; length; --length)
    for (const auto& x: memo[length])
      for (const auto& y: memo[size - length])
        insert(map, x, y);

  replicate_sqrt<Unsigned>(map);
  replicate_factorial<Unsigned>(map);

  return map;
}

} // namespace internal

template<typename Unsigned>
std::string find(std::size_t size, int digit, const Unsigned& target)
{
  typedef std::unordered_map< Integer<Unsigned>, std::unique_ptr<Expression>, std::hash<Unsigned> > Map;
  std::unique_ptr<Map[]> memo(new Map[size]);

  for (std::size_t length = 1; length <= size; ++length) {
    const Map& map = internal::build(memo.get() - 1, length, digit, target);
    auto found = map.find(target);

    if (found != map.end())
      return found->second->str();
  }

  return "";
}

} // namespace Chic

#endif // CHIC_HPP
