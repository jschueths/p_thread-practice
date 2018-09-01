//  Name: Joshua McCarville-Schueths
//	File: pthread_practice.cpp
//  Description: This program demonstrates creating threads,
//               passing data to threads, and using mutual
//               exclusion to prevent concurrency issues. The
//               way the program does that is by simulating
//               a shared set of savings and checking accounts
//               for a three member family.

#include <array>
#include <vector>
#include <string>
#include <thread>
#include "thread_practice.hpp"

std::ostream& operator<<(std::ostream& out, AccountType type) {
    if(type == AccountType::Checking) {
        out << "checking";
    } else if(type == AccountType::Savings) {
        out << "savings";
    }
    return out;
}

int main(int argc, char *argv[]) {
    // Declare thread id's and filename strings.
    std::vector<std::thread> tid;
    std::vector<std::string> files;

    // Make sure there are enough files entered.
    if (argc != 4) {
        std::cout << "Please make sure you have entered: " << std::endl;
        std::cout << "father.txt" << std::endl << "mother.txt" << std::endl;
        std::cout << "daughter.txt" << std::endl;
        return 1;
    }
    // Create 3 threads and pass the filenames to each one.
    for(size_t i=0; i < 3; i++) {
        files.emplace_back(argv[i+1]);
        tid.emplace_back(&family_member, files.back());
    }
    for(auto& t : tid) {
        // Have each thread rejoin after its execution.
        t.join();
    }
    return 0;
}

// Thread that simulates a family member.
void* family_member(const std::string& file_name) {
    if(file_name == "father.txt") {
        // If passed the father file, simulate the father.
        std::ifstream in(file_name);
        processTransactions("Bruce", in);
    } else if(file_name == "mother.txt") {
        // If passed the mother file, simulate the mother.
        std::ifstream in(file_name);
        processTransactions("Jennifer", in);
    } else if(file_name == "daughter.txt") {
        // If passed the daughter file, simulate the daughter.
        std::ifstream in(file_name);
        processTransactions("Jill", in);
    }
    return nullptr;
}

void processTransactions(const std::string& name, std::ifstream& in) {
    Transaction t;
    while(in >> t) {
        if(t.operation() == Operation::Deposit) {
            // Deposit routine.
            if(t.account() == AccountType::Checking) {
                std::lock_guard l(checkingLock);
                checking += t.amount();
                std::cout << name << " deposited $" << t.amount() << ". New "
                          << t.account() << " balance: $" << checking << std::endl;
            } else if(t.account() == AccountType::Savings) {
                std::lock_guard l(savingsLock);
                savings += t.amount();
                std::cout << name << " deposited $" << t.amount() << ". New "
                          << t.account() << " balance: $" << savings << std::endl;
            }
        } else if(t.operation() == Operation::Withdrawal) {
            // Withdraw routine.
            // Lock whichever account is being accessed.
            if(t.account() == AccountType::Checking) {
                std::lock_guard l(checkingLock);
                if(t.amount() > checking) {
                    // Lock both accounts if transfering.
                    std::lock_guard transferL(transferLock);
                    std::lock_guard savingsL(savingsLock);
                    if(savings > 0) {
                        savings += checking;
                        checking = 0;
                        savings -= t.amount();
                        std::cout << name << " withdrew $" << t.amount() << "." << std::endl;
                        printBalances();
                    } else {
                        std::cout << name << " cannot withdraw $" << t.amount()
                                  << ". Insufficient funds." << std::endl;
                    }
                } else {
                    checking -= t.amount();
                    std::cout << name << " withdrew $" << t.amount() << ". New "
                              << t.account() << " balance: $" << checking << std::endl;
                }
            } else if(t.account() == AccountType::Savings) {
                std::lock_guard l(savingsLock);
                if(savings > 0) {
                    savings -= t.amount();
                    std::cout << name << " withdrew $" << t.amount() << ". New"
                              << t.account() << " balance: $" << savings << std::endl;
                } else {
                    std::cout << name << " cannot withdraw $" << t.amount()
                              << ". Insufficient funds." << std::endl;
                }
            }
        } else if(t.operation() == Operation::Transfer) {
            // Transfer routine.
            // Lock both accounts when transfering.
            std::lock_guard tLock(transferLock);
            std::lock_guard cLock(checkingLock);
            std::lock_guard sLock(savingsLock);
            if(t.account() == AccountType::Checking) {
                if(savings > 0) {
                    checking += t.amount();
                    savings -= t.amount();
                    std::cout << name << " transferred $" << t.amount()
                              << " from savings to checking." << std::endl;
                    printBalances();
                } else {
                    std::cout << name << " cannot transfer $" << t.amount()
                              << ". Insufficient funds." << std::endl;
                }
            } else if(t.account() == AccountType::Savings) {
                if(checking > 0) {
                    checking -= t.amount();
                    savings += t.amount();
                    std::cout << name << " transferred $" << t.amount()
                              << " from checking to savings." << std::endl;
                    printBalances();
                } else {
                    std::cout << name << " cannot transfer $" << t.amount()
                              << ". Insufficient funds." << std::endl;
                }
            }
        }
    }
    return;
}

// Prints the balances of both accounts.
// Used when both accounts have been affected
// during a transaction.
void printBalances()
{
    std::cout << "Checking Balance: $" << checking << std::endl;
    std::cout << "Savings Balance: $" << savings << std::endl;
    return;
}

std::istream& operator>>(std::istream& in, Transaction& transaction) {
    char op;
    in >> op;
    if(op == 'd') {
        transaction.mOperation = Operation::Deposit;
    } else if(op == 'w') {
        transaction.mOperation = Operation::Withdrawal;
    } else if(op == 't') {
        transaction.mOperation = Operation::Transfer;
    } else {
        in.setstate(std::istream::failbit);
    }
    int accountId;
    in >> accountId;
    if(accountId == 0) {
        transaction.mAccount = AccountType::Checking;
    } else if(accountId == 1) {
        transaction.mAccount = AccountType::Savings;
    } else {
        in.setstate(std::istream::failbit);
    }
    in >> transaction.mAmount;
    return in;
}
