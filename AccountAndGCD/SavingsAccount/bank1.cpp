#include "alang.hpp"
#include <iostream>

class BankAccount : monitor {
    cond withdrawCondition;
    int balance;
    public:
        BankAccount(int balance): balance(balance) {}

        int withdraw(int amount) {
            SYNC;
            
            while (balance < amount) {
                logl("[W] WAITING FOR WITHDRAWAL - AMOUNT: ", amount);
                wait(withdrawCondition);
            }

            logl("[-] WITHDRAWING - AMOUNT: ", amount);
            balance -= amount; 
            //finished withdrawing, signal waiting withdrawals
            signal(withdrawCondition);

            return balance;
        }

        int deposit(int amount) {
            SYNC;

            logl("[+] DEPOSITING - AMOUNT: ", amount);
            balance += amount; 
            //balance has increased, signal waiting withdrawals
            signal(withdrawCondition);

            return balance;
        }
};


int main() {
    BankAccount b(0);

    {
        int N = 100;

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

    alang::logl("[$] BALANCE: ", b.withdraw(0));
    if (b.withdraw(0) != 0) throw "Balance differs from initial value";
}