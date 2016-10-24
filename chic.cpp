#include "Dictionary.hpp"
#include "Fraction.hpp"
#include <future>
#include <iostream>
#include <cstdint>

template<typename Wrapper, typename Unsigned>
static std::size_t find(const Unsigned& target, int digit, std::size_t limit = -1)
{
  Chic::Dictionary<Wrapper> dictionary(digit);

  if (dictionary.build(target, limit)) {
    std::cout << target << '#' << digit << ": "
      << dictionary.hierarchy().size() << " digits used\n--------------------\n"
      << dictionary.resolve(target) << std::endl;

    return dictionary.hierarchy().size();
  }

  return limit;
}

template<typename Unsigned>
static void find(const Unsigned& target, int digit)
{
  find<Chic::Fraction<Unsigned>>(target, digit, find<Chic::Integer<Unsigned>>(target, digit));
}

template<typename Unsigned>
static void run(const Unsigned& target)
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
