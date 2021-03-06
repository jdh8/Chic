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

#ifndef CHIC_ARITHMETIC_HPP
#define CHIC_ARITHMETIC_HPP

#include <iosfwd>

namespace Chic {

template<typename Derived>
struct Arithmetic
{
  friend Derived operator+(Derived x, Derived y) { return x += y; }
  friend Derived operator-(Derived x, Derived y) { return x -= y; }
  friend Derived operator*(Derived x, Derived y) { return x *= y; }
  friend Derived operator/(Derived x, Derived y) { return x /= y; }
};

} // namespace Chic

#endif // CHIC_ARITHMETIC_HPP
