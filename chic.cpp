#include "Dictionary.hpp"
#include "Entry.hpp"
#include "Expression.hpp"
#include "Fraction.hpp"
#include <iostream>
#include <sstream>
#include <cstdint>

template<typename Key, typename Unsigned>
static std::size_t find(Unsigned target, int digit, std::size_t limit = -1)
{
  Chic::Dictionary<Key> dictionary(digit);

  if (dictionary.build(target, limit)) {
    std::cout << target << '#' << digit << ": "
      << dictionary.level() << " digits used\n--------------------\n"
      << dictionary.tree(target) << std::endl;

    return dictionary.level();
  }

  return limit;
}

template<typename Unsigned>
static void find(Unsigned target, int digit)
{
  find<Chic::Fraction<Unsigned>>(target, digit, find<Chic::Entry<Unsigned>>(target, digit));
}

template<typename Unsigned>
static void run(Unsigned target)
{
  for (int digit = 1; digit <= 9; ++digit)
    find(target, digit);
}

int main(int argc, char** argv)
{
  std::ios_base::sync_with_stdio(false);

  if (argc == 2) {
    std::istringstream stream(argv[1]);
    std::uint_fast64_t target;
    stream >> target;
    run(target);
  }
  else {
    std::cout << "Usage: " << argv[0] << " TARGET\n\n"
      "TARGET  The result to make\n"
      "\n"
      "Syntactically correct but numerically invalid input\n"
      "causes undefined behavior.\n";
  }
}
