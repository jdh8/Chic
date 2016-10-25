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

#ifndef CHIC_FACTORIAL_HPP
#define CHIC_FACTORIAL_HPP

#include "Overflow.hpp"
#include <vector>

namespace Chic {

template<typename Integer>
class Factorial
{
  private:
    std::vector<Integer> _table;

  public:
    Factorial();
    Integer operator()(Integer) const;
};

template<typename Integer>
Factorial<Integer>::Factorial()
  : _table(1, 1)
{
  for (Integer k = 1; ; ++k)
  {
    Overflow<Integer> candidate = _table.back();

    if (candidate *= k)
      break;
    else
      _table.push_back(candidate);
  }
}

template<typename Integer>
Integer Factorial<Integer>::operator()(Integer n) const
{
  return n < _table.size() ? _table[n] : 0;
}

template<typename Integer>
Integer factorial(Overflow<Integer> n)
{
  static const Factorial<Integer> implementation;

  return implementation(n);
}

} // namespace Chic

#endif // CHIC_FACTORIAL_HPP
