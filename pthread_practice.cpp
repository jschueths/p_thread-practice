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
#include "pthread_practice.hpp"

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
    std::vector<pthread_t> tid;
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

        pthread_create(&tid[i], nullptr, family_member, static_cast<void*>(&files.back()));
    }
    for(const auto& t : tid) {
        // Have each thread rejoin after its execution.
        pthread_join(t, nullptr);
    }
    return 0;
}

// Thread that simulates a family member.
void* family_member(void* text_file) {
    auto file_name = *static_cast<std::string*>(text_file);
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
                pthread_mutex_lock(&checkingLock);
                checking += t.amount();
                std::cout << name << " deposited $" << t.amount() << ". New "
                          << t.account() << " balance: $" << checking << std::endl;
                pthread_mutex_unlock(&checkingLock);
            } else if(t.account() == AccountType::Savings) {
                pthread_mutex_lock(&savingsLock);
                savings += t.amount();
                std::cout << name << " deposited $" << t.amount() << ". New "
                          << t.account() << " balance: $" << savings << std::endl;
                pthread_mutex_unlock(&savingsLock);
            } else {
                // Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
            }
        } else if(t.operation() == Operation::Withdrawal) {
            // Withdraw routine.
            // Lock whichever account is being accessed.
            if(t.account() == AccountType::Checking) {
                pthread_mutex_lock(&checkingLock);
                if(t.amount() > checking) {
                    // Lock both accounts if transfering.
                    pthread_mutex_lock(&transferLock);
                    pthread_mutex_lock(&savingsLock);
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
                    pthread_mutex_unlock(&savingsLock);
                    pthread_mutex_unlock(&transferLock);
                } else {
                    checking -= t.amount();
                    std::cout << name << " withdrew $" << t.amount() << ". New "
                              << t.account() << " balance: $" << checking << std::endl;
                }
                pthread_mutex_unlock(&checkingLock);
            } else if(t.account() == AccountType::Savings) {
                pthread_mutex_lock(&savingsLock);
                {
                    if(savings > 0) {
                        savings -= t.amount();
                        std::cout << name << " withdrew $" << t.amount() << ". New"
                                  << t.account() << " balance: $" << savings << std::endl;
                    } else {
                        std::cout << name << " cannot withdraw $" << t.amount()
                                  << ". Insufficient funds." << std::endl;
                    }
                }
                pthread_mutex_unlock(&savingsLock);
            } else {
                // Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
            }
        } else if(t.operation() == Operation::Transfer) {
            // Transfer routine.
            // Lock both accounts when transfering.
            pthread_mutex_lock(&transferLock);
            pthread_mutex_lock(&checkingLock);
            pthread_mutex_lock(&savingsLock);
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
            } else {
                // Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
            }
            pthread_mutex_unlock(&savingsLock);
            pthread_mutex_unlock(&checkingLock);
            pthread_mutex_unlock(&transferLock);
        } else {
            // Error check for input info.
            std::cout << "Not a recognized operation" << std::endl;
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
