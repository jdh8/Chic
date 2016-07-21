#include "Chic.hpp"
#include <iostream>
#include <sstream>

template<typename Unsigned>
void program(const Unsigned& target, int limit = 10)
{
  for (int digit = 1; digit <= 9; ++digit)
    std::cout << Chic::find(limit, digit, target).str() << std::endl;
}

template<typename Unsigned>
void program(const char* first)
{
  std::istringstream stream(first);
  Unsigned target;
  stream >> target;
  program(target);
}

template<typename Unsigned>
void program(const char* first, const char* second)
{
  std::istringstream stream(first);
  Unsigned target;
  int limit;

  stream >> target;

  stream.clear();
  stream.str(second);
  stream >> limit;

  program(target, limit);
}

int main(int argc, char** argv)
{
  typedef unsigned int Unsigned;
  std::ios_base::sync_with_stdio(false);

  switch (argc) {
    case 2:
      program<Unsigned>(argv[1]);
      return 0;
    case 3:
      program<Unsigned>(argv[1], argv[2]);
      return 0;
  }

  std::cout << "Usage: " << argv[0] << " TARGET [LIMIT = 10]\n\n"
    "TARGET  The result to make\n"
    "LIMIT   Maximum number of digits\n"
    "\n"
    "Syntactically correct but numerically invalid input\n"
    "causes undefined behavior.\n";
    
  return 1;
}
