#include "Fraction.hpp"
#include <random>
#include <cassert>

int main()
{
  typedef Chic::Fraction<std::uint_fast64_t> Fraction;

  std::mt19937_64 random { std::random_device()() };

  Fraction x(random(), random());
  Fraction y(random(), random());

  Fraction sum = x + y;
  Fraction product = x * y;
  Fraction square = x.square();

  assert(!std::isfinite(sum) || sum - x == y);
  assert(!(y && std::isfinite(product)) || product / y == x);
  assert(!std::isfinite(square) || square.sqrt() == x);
}
