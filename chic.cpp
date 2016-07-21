#include "Chic.hpp"
#include <iostream>
#include <sstream>
#include <cstdint>

void program(std::uint64_t target, int limit = 10)
{
  for (int digit = 1; digit <= 9; ++digit)
    std::cout << Chic::find(limit, digit, target).str() << std::endl;
}

void program(const char* first)
{
  std::istringstream stream(first);
  std::uint64_t target;
  stream >> target;
  program(target);
}

void program(const char* first, const char* second)
{
  std::istringstream stream(first);
  std::uint64_t target;
  int limit;

  stream >> target;

  stream.clear();
  stream.str(second);
  stream >> limit;

  program(target, limit);
}

int main(int argc, char** argv)
{
  std::ios_base::sync_with_stdio(false);

  switch (argc) {
    case 2:
      program(argv[1]);
      return 0;
    case 3:
      program(argv[1], argv[2]);
      return 0;
  }

  std::cout << "Usage: " << argv[0] << " TARGET [LIMIT = 10]\n\n"
    "TARGET must be a positive 64-bit integer.\n" 
    "LIMIT must be a positive integer.\n"
    "\n"
    "Syntactically correct but numerically invalid input\n"
    "causes undefined behavior.\n";
    
  return 1;
}
