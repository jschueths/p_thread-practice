//  Name: Joshua McCarville-Schueths
//	File: pthread_practice.h
//  Description: This program demonstrates creating threads,
//							 passing data to threads, and using mutual
//							 exclusion to prevent concurrency issues. The
//							 way the program does that is by simulating
//         			 a shared set of savings and checking accounts
//							 for a three member family.


#include <fstream>
#include <iostream>
#include <string>
#include <mutex>

// Thread that is allows parallel processing.
void* family_member(const std::string& file_name);

// Reads a transaction file and applies each one.
void processTransactions(const std::string& name, std::ifstream& in);
// Prints the current account balances.
void printBalances();

// Global Variables for Threads //
int savings = 0; // Savings account value.
int checking = 0; // Checking account value.
std::mutex savingsLock; // Mutex for savings.
std::mutex checkingLock; // Mutex for checking.
std::mutex transferLock; // Mutex to prevent transfer deadlock.

class Transaction;
std::istream& operator>>(std::istream& in, Transaction& transaction);

enum class Operation {
    Deposit,
    Withdrawal,
    Transfer
};

enum class AccountType {
    Checking,
    Savings
};

std::ostream& operator<<(std::ostream& out, AccountType type);

class Transaction {
    public:

        Operation operation() const { return mOperation; }
        AccountType account() const { return mAccount; }
        int amount() const { return mAmount; }
        friend std::istream& operator>>(std::istream& in, Transaction& transaction);

    private:
        Operation mOperation {Operation::Deposit};
        AccountType mAccount {AccountType::Checking};
        int mAmount {0};
};

