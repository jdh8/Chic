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

template<typename Map, typename Pair>
void insert(Map& map, const Pair& pair)
{
  if (pair.first) {
    Expression& reference = map[pair.first];

    if (reference.str().empty() || pair.second.str().size() < reference.str().size())
      reference = pair.second;
  }
}

template<typename Map, typename Pair>
void insert(Map& map, const Pair& x, const Pair& y)
{
  insert(map, Pair(x.first + y.first, x.second + y.second));
  insert(map, Pair(x.first * y.first, x.second * y.second));
  insert(map, Pair(pow(x.first, y.first), pow(x.second, y.second)));
  insert(map, Pair(pow(y.first, x.first), pow(y.second, x.second)));

  insert(map, Pair(x.first - y.first, x.second - y.second));
  insert(map, Pair(y.first - x.first, y.second - x.second));
  insert(map, Pair(x.first / y.first, x.second / y.second));
  insert(map, Pair(y.first / x.first, y.second / x.second));
}

template<typename Map>
void replicate_sqrt(Map& map)
{
  typedef std::pair<typename Map::key_type, typename Map::mapped_type> Pair;

  for (const Pair& x: std::vector<Pair> (map.begin(), map.end())) {
    for (Pair y(sqrt(x.first), sqrt(x.second)); y.first > 1; ) {
      map.insert(y);
      y.first = sqrt(y.first);
      y.second = sqrt(y.second);
    }
  }
}

template<typename Map>
void replicate_factorial(Map& map)
{
  typedef std::pair<typename Map::key_type, typename Map::mapped_type> Pair;

  for (const Pair& x: std::vector<Pair> (map.begin(), map.end())) {
    for (Pair y(factorial(x.first), factorial(x.second)); y.first > 2; ) {
      map.insert(y);
      y.first = factorial(y.first);
      y.second = factorial(y.second);
    }
  }
}

template<typename Map, typename Unsigned>
const Map& build(Map* memo, std::size_t size, int digit, const Unsigned& target)
{
  Map& map = memo[size];

  map[Integer<Unsigned>(size, digit)] = Expression(size, digit);

  for (std::size_t length = size / 2; length; --length)
    for (const auto& x: memo[length])
      for (const auto& y: memo[size - length])
        insert(map, x, y);

  replicate_sqrt(map);
  replicate_factorial(map);

  return map;
}

} // namespace internal

template<typename Unsigned>
Expression find(std::size_t size, int digit, const Unsigned& target)
{
  typedef std::unordered_map<Integer<Unsigned>, Expression, std::hash<Unsigned>> Map;
  std::unique_ptr<Map[]> memo(new Map[size]);

  for (std::size_t length = 1; length <= size; ++length) {
    const Map& map = internal::build(memo.get() - 1, length, digit, target);
    auto found = map.find(target);

    if (found != map.end())
      return found->second;
  }

  return Expression();
}

} // namespace Chic

#endif // CHIC_HPP
