#include "Dictionary.hpp"
#include <iostream>
#include <cstdint>

template<typename Unsigned>
static void run(const Unsigned& target)
{
  using namespace Chic;

  for (int digit = 1; digit <= 9; ++digit) {
    Dictionary<Unsigned> dictionary(digit);
    const Expression<Unsigned>& found = dictionary.build(target);
    std::cout << found.resolve(dictionary) << std::endl;
  }
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
