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

#include "Integer.hpp"
#include "Overflow.hpp"
#include <queue>
#include <stack>
#include <unordered_map>
#include <vector>

namespace Chic {

template<typename> class Expression;
template<typename> class Entry;
template<typename> class Fraction;

template<typename> struct Reservation;

template<typename Unsigned>
struct Reservation<Entry<Unsigned>>
{
  static const std::size_t size = std::size_t(std::numeric_limits<Unsigned>::digits) << 19;
};

template<typename Unsigned>
struct Reservation<Fraction<Unsigned>>
{
  static const std::size_t size = std::size_t(std::numeric_limits<Unsigned>::digits) << 20;
};

template<typename Key>
class Dictionary
{
  private:
    std::unordered_map<Key, Expression<Key>> _graph;
    std::vector<std::vector<Key>> _hierarchy;

    bool _basic(Key, Expression<Key>);
    void _quadratic(Key, Expression<Key>);
    void _factorial();

    template<typename Unsigned>
    void _divides(Entry<Unsigned>, Entry<Unsigned>);

    template<typename Other>
    void _divides(Other, Other);

    template<typename Unsigned>
    void _pow(Entry<Unsigned>, Entry<Unsigned>);

    template<typename Unsigned>
    void _pow(Fraction<Unsigned>, Fraction<Unsigned>);

    void _binary(Key, Key);
    void _neighbors(Key, Key);

  public:
    const int digit;

    Dictionary(int);

    void grow();
    bool build(Key, std::size_t limit = -1);
    std::size_t level() const;

    template<typename Container, typename Function>
    Function bfs(Key, Function) const;

    template<typename Function>
    Function bfs(Key, Function) const;

    template<typename Container, typename Function>
    Function dfs(Key, Function) const;

    template<typename Function>
    Function dfs(Key, Function) const;
};

template<typename Key>
Dictionary<Key>::Dictionary(int strain) :
  #ifndef __APPLE__
    _graph(Reservation<Key>::size),
  #endif
    digit(strain)
{}

template<typename Key>
bool Dictionary<Key>::_basic(Key key, Expression<Key> expression)
{
  bool status = key && _graph.emplace(key, expression).second;

  if (status)
    _hierarchy.back().emplace_back(key);

  return status;
}

template<typename Key>
void Dictionary<Key>::_quadratic(Key key, Expression<Key> expression)
{
  while (_basic(key, expression))
  {
    expression = { key, 's' };
    key = key.sqrt();
  }
}

template<typename Key>
void Dictionary<Key>::_factorial()
{
  std::vector<Key>& destination = _hierarchy.back();
  std::size_t length = destination.size();

  for (std::size_t k = 0; k < length; ++k)
  {
    Key x = destination[k];
    Key y = x.factorial();

    while (_basic(y, { x, '!' }))
    {
      x = y;
      y = y.factorial();
    }
  }
}

template<typename Key>
template<typename Unsigned>
void Dictionary<Key>::_divides(Entry<Unsigned> x, Entry<Unsigned> y)
{
  _quadratic(x / y, { x, y, '/' });
  _quadratic(y / x, { y, x, '/' });
}

template<typename Key>
template<typename Other>
void Dictionary<Key>::_divides(Other x, Other y)
{
  Other quotient = x / y;

  _quadratic(quotient, { x, y, '/' });
  _quadratic(quotient.inverse(), { y, x, '/' });
}

template<typename Key>
template<typename Unsigned>
void Dictionary<Key>::_pow(Entry<Unsigned> x, Entry<Unsigned> y)
{
  if (x > 1 && y)
  {
    int shift = ctz(y.value());
    Unsigned odd = y >> shift;

    if (odd >= std::numeric_limits<Unsigned>::digits)
      return;

    Entry<Unsigned> base = x.pow(odd);
    Entry<Unsigned> sqrt = base.sqrt();

    _quadratic(sqrt, { x, y, shift + 2 });

    while (shift >= 0 && base)
    {
      _basic(base, { x, y, shift + 1 });

      base *= base;
      --shift;
    }
  }
}

template<typename Key>
template<typename Unsigned>
void Dictionary<Key>::_pow(Fraction<Unsigned> x, Fraction<Unsigned> y)
{
  if (y.den() == 1 && x && x.num() != x.den())
  {
    int shift = ctz(y.num());
    Unsigned odd = y.num() >> shift;

    if (odd >= std::numeric_limits<Unsigned>::digits)
      return;

    Fraction<Unsigned> base = x.pow(odd);
    Fraction<Unsigned> sqrt = base.sqrt();

    _quadratic(sqrt, { x, y, shift + 2 });
    _quadratic(sqrt.inverse(), { x, y, -(shift + 2) });

    while (shift >= 0 && base)
    {
      _basic(base, { x, y, shift + 1 });
      _basic(base.inverse(), { x, y, -(shift + 1) });

      base = base.square();
      --shift;
    }
  }
}

template<typename Key>
void Dictionary<Key>::_binary(Key x, Key y)
{
  _quadratic(x + y, { x, y, '+' });
  _quadratic(x * y, { x, y, '*' });

  _quadratic(x - y, { x, y, '-' });
  _quadratic(y - x, { y, x, '-' });

  _divides(x, y);

  _pow(x, y);
  _pow(y, x);

  _quadratic(x.factorial(y), { x, y, '!' });
  _quadratic(y.factorial(x), { y, x, '!' });
}

template<typename Key>
void Dictionary<Key>::_neighbors(Key x, Key y)
{
  if (Key ratio = x.factorial(y))
  {
    _quadratic(ratio + Key(1), { x, y, '!' + 1 });
    _quadratic(ratio - Key(1), { x, y, '!' - 1 });
  }
}

template<typename Key>
void Dictionary<Key>::grow()
{
  _hierarchy.emplace_back();

  std::size_t size = level();
  Key root(size, digit);

  _quadratic(root, root);

  for (std::size_t length = size / 2; length > 0; --length)
    for (Key x: _hierarchy[length - 1])
      for (Key y: _hierarchy[size - length - 1])
        _binary(x, y);

  if (size >= 3)
    for (Key x: _hierarchy[size - 3])
      for (Key y: _hierarchy[0])
        _neighbors(x, y);

  _factorial();
}

template<typename Key>
bool Dictionary<Key>::build(Key key, std::size_t limit)
{
  while (_hierarchy.size() < limit)
  {
    auto found = _graph.find(key);

    if (found != _graph.end())
      return true;

    grow();
  }

  return false;
}

template<typename Key>
std::size_t Dictionary<Key>::level() const
{
  return _hierarchy.size();
}

template<typename Key>
template<typename Container, typename Function>
Function Dictionary<Key>::bfs(Key key, Function f) const
{
  Container container = { key };

  for (std::queue<Key, Container> queue(container); !queue.empty(); queue.pop())
  {
    key = queue.front();
    Expression<Key> expression = _graph.at(key);

    if (expression.symbol())
    {
      if (expression.second())
        queue.push(expression.second());

      queue.push(expression.first());
      f(key, expression);
    }
  }

  return f;
}

template<typename Key>
template<typename Function>
Function Dictionary<Key>::bfs(Key key, Function f) const
{
  return bfs<std::deque<Key>>(key, f);
}

template<typename Key>
template<typename Container, typename Function>
Function Dictionary<Key>::dfs(Key key, Function f) const
{
  Container container = { key };

  for (std::stack<Key, Container> stack(container); !stack.empty(); stack.pop())
  {
    key = stack.top();
    Expression<Key> expression = _graph.at(key);

    if (expression.symbol())
    {
      if (expression.second())
        stack.push(expression.second());

      stack.push(expression.first());
      f(key, expression);
    }
  }

  return f;
}

template<typename Key>
template<typename Function>
Function Dictionary<Key>::dfs(Key key, Function f) const
{
  return dfs<std::vector<Key>>(key, f);
}

} // namespace Chic

#endif // CHIC_DICTIONARY_HPP
