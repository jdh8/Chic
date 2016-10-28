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

#ifndef CHIC_BREAKDOWN_HPP
#define CHIC_BREAKDOWN_HPP

#include <unordered_set>
#include <iosfwd>

namespace Chic {

template<typename> class Expression;

template<typename Key>
class Breakdown
{
  private:
    std::unordered_set<Key> _memo;
    std::ostream& _stream;

  public:
    explicit Breakdown(std::ostream&);
    std::ostream& operator()(Key, Expression<Key>);
};

template<typename Key>
Breakdown<Key>::Breakdown(std::ostream& stream)
  : _stream(stream)
{}

template<typename Key>
std::ostream& Breakdown<Key>::operator()(Key key, Expression<Key> expression)
{
  return _memo.insert(key).second ? _stream << key << " = " << expression << '\n' : _stream;
}

template<typename Key>
static Breakdown<Key> breakdown(std::ostream& stream)
{
  return Breakdown<Key>(stream);
}

} // namespace Chic

#endif // CHIC_BREAKDOWN_HPP
