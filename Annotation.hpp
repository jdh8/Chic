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

#ifndef CHIC_ANNOTATION_HPP
#define CHIC_ANNOTATION_HPP

#include <type_traits>

namespace Chic {

template<typename Character>
class Annotation
{
  private:
    typedef typename std::make_signed<Character>::type Signed;

    Character _base;
    Signed _annotation;

  public:
    Annotation(Character = 0, Signed = 0);
};

template<typename Character>
Annotation<Character>::Annotation(Character base, Signed annotation)
  : _base(base),
    _annotation(annotation)
{}

} // namespace Chic

#endif // CHIC_ANNOTATION_HPP
