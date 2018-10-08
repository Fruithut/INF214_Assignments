#include "alang.hpp"
#include <iostream>

class BankAccount : monitor {
    semaphore balanceMutex = 1;
    semaphore queueNumMutex = 1;
    cond withdrawCondition;
    int balance;
    int queueNumber = 0;
    int queueNumberInTurn = 0;
public:
    BankAccount(int balance): balance(balance) {}

    int withdraw(int amount) {
        SYNC;

        // pick queue number
        queueNumMutex.P();
            int localQueueNumber = queueNumber;
            queueNumber++;
        queueNumMutex.V();

        while (balance < amount || localQueueNumber != queueNumberInTurn) {
            alang::logl("[W] WAITING FOR WITHDRAWAL - AMOUNT: ", amount, " QUEUE NUM: ", localQueueNumber, " CURRENT QUEUE NR: ", queueNumberInTurn);
            wait(withdrawCondition);
        }

        // withdraw and set next queue number's turn
        balanceMutex.P();
            alang::logl("[-] WITHDRAWING - AMOUNT: ", amount, " QUEUE NUM OF WITHDRAWAL: ", localQueueNumber);
            balance -= amount;
            queueNumberInTurn++;
        balanceMutex.V();

        // finished withdrawing, signal waiting withdrawals
        signal_all(withdrawCondition);
        return balance;
    }

    int deposit(int amount) {
        SYNC;
        balanceMutex.P();
            alang::logl("[+] DEPOSITING - AMOUNT: ", amount);
            balance += amount;
        balanceMutex.V();

        // balance has increased, signal waiting withdrawals
        signal_all(withdrawCondition);
        return balance;
    }
};


int main() {
    BankAccount b(200);

    {
        processes ps;
        ps += [&] {
            b.withdraw(300);
        };
        ps += [&] {
            b.withdraw(100);
        };
        ps += [&] {
            b.deposit(200);
        };
        ps += [&] {
            b.withdraw(500);
        };
        ps += [&] {
            b.withdraw(300);
        };
        ps += [&] {
            b.deposit(1000);
        };
    }
    //initial balance should remain unchanged after processes have finished

    alang::logl("[$] BALANCE: ", b.withdraw(0));
    //if (b.withdraw(0) != 0) throw "Balance differs from initial value";
}