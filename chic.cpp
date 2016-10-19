#include "Dictionary.hpp"
#include "Fraction.hpp"
#include <future>
#include <iostream>
#include <cstdint>

#include <unordered_set>
template<typename Unsigned>
static std::string find(const Unsigned& target, int digit)
{
  Chic::Dictionary<Unsigned> dictionary(digit);
  return dictionary.resolve(dictionary.build(target));
}

template<typename Unsigned>
static void run(const Unsigned& target)
{
  std::future<std::string> message[10];

  for (int digit = 1; digit <= 9; ++digit)
    message[digit] = std::async(find<Unsigned>, target, digit);

  for (int digit = 1; digit <= 9; ++digit)
    std::cout << message[digit].get() << std::endl;
}

int main(int argc, char** argv)
{
  std::ios_base::sync_with_stdio(false);

  Chic::Fraction<std::size_t> a(3), b(7);

  std::cout << std::string(a + b) << std::endl;

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
