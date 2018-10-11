#include "alang.hpp"
#include <iostream>

int gcd(int n, int m) {
    A<int> x = n;
    A<int> y = m;
    while (x != y) {
      CO([&]{ ATO if (x > y)  x = x - y; MIC; },
         [&]{ ATO if (x < y)  y = y - x; MIC; });
    }

    logl("GCD OF: ", n, " & ", m, " = ", x);
    return x;
}

int main() {
    // test a few results
    int result1 = gcd(2,3);
    assert(result1 == 1);
    int result2 = gcd(25235, 2345);
    assert(result2 == 35);
    int result3 = gcd(-310, -150);
    assert(result3 == 10);
    
    // this will not terminate!
    //log(gcd(-10,30));
}