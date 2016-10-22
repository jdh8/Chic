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
#include "IO.hpp"
#include <unordered_map>
#include <vector>

namespace Chic {

template<typename>
class Integer;

template<typename Number>
class Dictionary
{
  private:
    class Resolver;

    std::unordered_map<Number, Expression<Number>> _graph;
    std::vector<std::vector<Number>> _hierarchy;
    const int _digit;

    void emplace(Number, Expression<Number>);
    void factorial();

    template<typename Unsigned>
    void divides(const Integer<Unsigned>&, const Integer<Unsigned>&);

    template<typename Default>
    void divides(const Default&, const Default&);

    template<typename Unsigned>
    void pow(const Integer<Unsigned>&, const Integer<Unsigned>&);

    template<typename Default>
    void pow(const Default&, const Default&);

    void binary(const Number&, const Number&);

  public:
    Dictionary(int);

    const Expression<Number>& operator[](const Number&) const;
    const std::unordered_map<Number, Expression<Number>>& graph() const;
    const std::vector<std::vector<Number>>& hierarchy() const;
    int digit() const;
    Resolver resolve(const Number&) const;

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
void Dictionary<Number>::emplace(Number key, Expression<Number> expr)
{
  while (key && _graph.emplace(key, expr).second) {
    _hierarchy.back().emplace_back(key);

    expr = { key, 's' };
    key = key.sqrt();
  }
}

template<typename Number>
void Dictionary<Number>::factorial()
{
  auto& destination = _hierarchy.back();
  std::size_t length = destination.size();

  for (std::size_t k = 0; k < length; ++k) {
    auto x = destination[k];
    auto y = x.factorial();

    // Due to Bertrand's postulate, from 2! on, factorials cannot be a perfect square.
    while (y && _graph.emplace(y, Expression<Number>(x, '!')).second) {
      destination.emplace_back(y);
      x = y;
      y = y.factorial();
    }
  }
}

template<typename Number>
template<typename Unsigned>
void Dictionary<Number>::divides(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
{
  emplace(x / y, { x, y, '/' });
  emplace(y / x, { y, x, '/' });
}

template<typename Number>
template<typename Default>
void Dictionary<Number>::divides(const Default& x, const Default& y)
{
  Default quotient = x / y;

  emplace(quotient, { x, y, '/' });
  emplace(quotient.inverse(), { y, x, '/' });
}

template<typename Number>
template<typename Unsigned>
void Dictionary<Number>::pow(const Integer<Unsigned>& x, const Integer<Unsigned>& y)
{
  emplace(x.pow(y), { x, y, '^' });
  emplace(y.pow(x), { y, x, '^' });
}

template<typename Number>
template<typename Default>
void Dictionary<Number>::pow(const Default& x, const Default& y)
{
  Default cache = x.pow(y);

  emplace(cache, { x, y, '^' });
  emplace(cache.inverse(), { x, y, -'^' });

  cache = y.pow(x);

  emplace(cache, { y, x, '^' });
  emplace(cache.inverse(), { y, x, -'^' });
}

template<typename Number>
void Dictionary<Number>::binary(const Number& x, const Number& y)
{
  emplace(x + y, { x, y, '+' });
  emplace(x * y, { x, y, '*' });

  emplace(x - y, { x, y, '-' });
  emplace(y - x, { y, x, '-' });

  divides(x, y);
  pow(x, y);
}

template<typename Number>
void Dictionary<Number>::grow()
{
  _hierarchy.emplace_back();

  const auto* base = _hierarchy.data() - 1;
  std::size_t size = _hierarchy.size();
  Number root(size, _digit);

  emplace(root, root);

  for (auto length = size / 2; length > 0; --length)
    for (const auto& x: base[length])
      for (const auto& y: base[size - length])
        binary(x, y);

  factorial();
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

template<typename Number>
typename Dictionary<Number>::Resolver Dictionary<Number>::resolve(const Number& key) const
{
  return { *this, key };
}

template<typename Number>
class Dictionary<Number>::Resolver : public IO<Dictionary<Number>::Resolver>
{
  private:
    const Dictionary& _dict;
    const Number& _key;

  public:
    Resolver(const Dictionary&, const Number&);

    template<typename Character>
    std::basic_ostream<Character>& operator()(std::basic_ostream<Character>&) const;
};

template<typename Number>
Dictionary<Number>::Resolver::Resolver(const Dictionary& dict, const Number& key)
  : _dict(dict),
    _key(key)
{}

template<typename Number>
template<typename Character>
std::basic_ostream<Character>& Dictionary<Number>::Resolver::operator()(std::basic_ostream<Character>& stream) const
{
  const auto& expr = _dict[_key];

  if (expr.symbol()) {
    stream << _key << " = " << expr << '\n' << Resolver(_dict, expr.first());

    if (expr.second())
      stream << Resolver(_dict, expr.second());
  }

  return stream;
}

} // namespace Chic

#endif // CHIC_DICTIONARY_HPP
