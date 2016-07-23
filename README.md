Chic
====
Tchisla is a math puzzle to make a target number with only one kind of digits,
operators +, -, \*, /, ^, !, √, and parentheses.  The fewer the digits, the
higher the score.

This software finds an answer with the fewest digits.  The operator √ is
precedent to ! because factorials from 2! on are no perfect squares.

Here comes the sample output solving 2016.

    (((1 + 1) ^ 11) - √((1 + 1) ^ (11 - 1)))
    ((2 * (2 + 2))! / (22 - 2))
    (3!! + (3! * (3! ^ 3)))
    ((4 + 4)! / (4! - 4))
    (((5 + 5)! / 5!) / (5 + (5 + 5)))
    (6! + (6 * √(6 ^ 6)))
    ((7! + 7!) / (7 - ((7 + 7) / 7)))
    (8! / ((8 + 8) + √(8 + 8)))
    ((9! + (√9 * 9!)) / √9!!)

Library
-------
This is a header-only library.  Include `Dictionary.hpp` for the main class
template `Chic::Dictionary`.

Command-line program
--------------------
Compile `chic.cpp`.

License
-------
GPLv3, because this software seems to be the first public implementation.
