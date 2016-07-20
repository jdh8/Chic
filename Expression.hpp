#ifndef CHIC_EXPRESSION_HPP
#define CHIC_EXPRESSION_HPP

#include <utility>
#include <string>

namespace Chic {

template<typename Unsigned>
struct Expression
  : std::pair<Unsigned, std::string>
{
  Expression();
  Expression(const Unsigned&, const std::string&);
  Expression(std::size_t, int);
};

template<typename Unsigned>
Expression<Unsigned>::Expression()
  : std::pair<Unsigned, std::string>()
{}

template<typename Unsigned>
Expression<Unsigned>::Expression(const Unsigned& value, const std::string& expression)
  : std::pair<Unsigned, std::string>(value, expression)
{}

template<typename Unsigned>
Expression<Unsigned>::Expression(std::size_t repeats, int digit)
  : std::pair<Unsigned, std::string>(0, std::string(repeats, '0'|digit))
{
  while (repeats--)
    this->first = 10 * this->first + digit;
}

template<typename Unsigned>
Expression<Unsigned> operator+(const Expression<Unsigned>& x, const Expression<Unsigned>& y)
{
  Expression<Unsigned> result(x.first + y.first, "");

  if (x.first < result.first)
    result.second = "(" + x.second + " + " + y.second + ")";
  else
    result.first = 0;

  return result;
}

template<typename Unsigned>
Expression<Unsigned> operator-(const Expression<Unsigned>& x, const Expression<Unsigned>& y)
{
  Expression<Unsigned> result;

  if (x.first > y.first) {
    result.first = x.first - y.first;
    result.second = "(" + x.second + " - " + y.second + ")";
  }
  return result;
}

template<typename Unsigned>
Expression<Unsigned> operator*(const Expression<Unsigned>& x, const Expression<Unsigned>& y)
{
  Expression<Unsigned> result(x.first * y.first, "");

  if (x.first && result.first / x.first == y.first)
    result.second = "(" + x.second + " * " + y.second + ")";
  else
    result.first = 0;

  return result;
}

template<typename Unsigned>
Expression<Unsigned> operator/(const Expression<Unsigned>& x, const Expression<Unsigned>& y)
{
  Expression<Unsigned> result;

  if (x.first % y.first == 0) {
    result.first = x.first / y.first;
    result.second = "(" + x.second + " / " + y.second + ")";
  }
  return result;
}

template<typename Unsigned>
Expression<Unsigned> pow(const Expression<Unsigned>& x, const Expression<Unsigned>& y)
{
  Expression<Unsigned> result(1, "(" + x.second + " ^ " + y.second + ")");
  Unsigned base = x.first;

  if (!(base && y.first))
    return Expression<Unsigned>();

  for (Unsigned exponent = y.first; exponent; exponent >>= 1) {
    if (exponent & 1) {
      Unsigned cache = result.first;
      result.first *= base;

      if (result.first / base != cache)
        return Expression<Unsigned>();
    }

    Unsigned cache = base;
    base *= base;

    if (base / cache != cache)
      return Expression<Unsigned>();
  }

  return result;
}

} // namespace Chic

#endif // CHIC_EXPRESSION_HPP
