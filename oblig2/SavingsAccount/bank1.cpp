#include "alang.hpp"
#include <iostream>

class BankAccount : monitor {
    semaphore balanceMutex = 1;
    cond withdrawCondition;
    int balance;
    public:
        BankAccount(int balance): balance(balance) {}

        int withdraw(int amount) {
            SYNC;
            while (balance < amount) {
                wait(withdrawCondition);
            }

            balanceMutex.P(); balance = balance - amount; balanceMutex.V();

            //finished withdrawing, signal waiting withdrawals
            signal(withdrawCondition);

            return balance;
        }

        int deposit(int amount) {
            SYNC;
            balanceMutex.P(); balance = balance + amount; balanceMutex.V();

            //balance has increased, signal waiting withdrawals
            signal(withdrawCondition);

            return balance;
        }
};


int main() {
    BankAccount b(0);

    {
        int N = 10000;

        processes ps;
        for (int i = 0; i < N; ++i) {
            ps += [&] {
                b.deposit(1);
            };
            ps += [&] {
                b.withdraw(1);
            };
        }
    }
    //initial balance should remain unchanged after processes have finished

    alang::logl("Balance: ", b.withdraw(0));
    if (b.withdraw(0) != 0) throw "Balance differs from initial value";
}