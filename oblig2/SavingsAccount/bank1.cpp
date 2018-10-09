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
            
            balanceMutex.P();
            while (balance < amount) {
                logl("[W] WAITING FOR WITHDRAWAL - AMOUNT: ", amount);
                balanceMutex.V();
                wait(withdrawCondition);
                balanceMutex.P();
            }

            logl("[-] WITHDRAWING - AMOUNT: ", amount);
            balance -= amount; 
            balanceMutex.V();

            //finished withdrawing, signal waiting withdrawals
            signal(withdrawCondition);

            return balance;
        }

        int deposit(int amount) {
            SYNC;
            balanceMutex.P(); 
            logl("[+] DEPOSITING - AMOUNT: ", amount);
            balance += amount; 
            balanceMutex.V();

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