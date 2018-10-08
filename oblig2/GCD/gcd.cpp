#include "alang.hpp"
#include <iostream>

int gcd(int n, int m) {
    A<int> x = n;
    A<int> y = m;
    while (n != m) {
      CO([&]{ ATO if (x > y)  x = x - y; MIC; },
         [&]{ ATO if (x < y)  y = y - x; MIC; });
    }

    return x;
}

int main() {
  logl(gcd(2, 3));
  logl(gcd(25235, 2345));
  logl(gcd(310, 150));
}