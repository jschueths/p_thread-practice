//  Name: Joshua McCarville-Schueths
//	File: pthread_practice.cpp
//  Description: This program demonstrates creating threads,
//							 passing data to threads, and using mutual
//							 exclusion to prevent concurrency issues. The
//							 way the program does that is by simulating
//         			 a shared set of savings and checking accounts
//							 for a three member family.

#include <array>
#include <string>
#include "pthread_practice.hpp"

int main(int argc, char *argv[]) {
	// Declare thread id's and filename strings.
	pthread_t tid[3];
    std::array<std::string, 3> file;
	
	// Make sure there are enough files entered.
    if (argc != 4) {
        std::cout << "Please make sure you have entered: " << std::endl;
        std::cout << "father.txt" << std::endl << "mother.txt" << std::endl;
        std::cout << "daughter.txt" << std::endl;
    } else {
		// Create 3 threads and pass the filenames to each one.
        for(size_t i=0; i < 3; i++) {
            file[i] = argv[i];
            pthread_create(&tid[i], nullptr, family_member, static_cast<void*>(&file[i]));
		}
        for(const auto& t : tid) {
			// Have each thread rejoin after its execution.
            pthread_join(t, nullptr);
		}
	}
	return 0;
}

// Thread that simulates a family member.
void* family_member(void* text_file) {
    auto file_name = static_cast<std::string*>(text_file);
    if(*(file_name) == "father.txt") {
		// If passed the father file, simulate the father.
		father();
    } else if(*(file_name) == "mother.txt") {
		// If passed the mother file, simulate the mother.
		mother();
    } else if(*(file_name) == "daughter.txt") {
		// If passed the daughter file, simulate the daughter.
		daughter();
	}
    return nullptr;
}

// Function that simulates the father.
void father()
{
    std::string accountName;
	char operation;
	int account;
	int amount;
    std::ifstream inFile;
	inFile.open("father.txt");
    while(!inFile.eof()) {
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
        if(account == 0) { // This block interprets the number
			accountName = "checking";
        } else if (account == 1) {
            accountName = "savings";
		}
		
        if(operation == 'd') {
			// Deposit routine for father.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
                std::cout << "Bruce deposited $" << amount << ". New "
                          << accountName << " balance: $" << checking << std::endl;
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
                pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
                std::cout << "Bruce deposited $" << amount << ". New "
                          << accountName << " balance: $" << savings << std::endl;
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}
        } else if(operation == 'w') {
			// Withdraw routine for father.
			// Lock whichever account is being accessed.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
                if(amount > checking) {
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
                    if(savings > 0) {
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
                        std::cout << "Bruce withdrew $" << amount << "." << std::endl;
						printBalances();
                    } else {
                        std::cout << "Bruce cannot withdraw $" << amount
                                  << ". Insufficient funds." << std::endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
                } else {
					checking = checking - amount;
                    std::cout << "Bruce withdrew $" << amount << ". New "
                              << accountName << " balance: $" << checking << std::endl;
				}
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
				pthread_mutex_lock(&savingsLock);
				{
                    if(savings > 0) {
						savings = savings - amount;
                        std::cout << "Bruce withdrew $" << amount << ". New"
                                  << accountName << " balance: $" << savings << std::endl;
                    } else {
                        std::cout << "Bruce cannot withdraw $" << amount <<
                                     ". Insufficient funds." << std::endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}	
        } else if(operation == 't') {
			// Transfer routine for father.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
            if(account == 0) {
                if(savings > 0) {
					checking = checking + amount;
					savings = savings - amount;
                    std::cout << "Bruce transferred $" << amount <<
                                 " from savings to checking." << std::endl;
					printBalances();
                } else {
                    std::cout << "Bruce cannot transfer $" << amount
                              << ". Insufficient funds." << std::endl;
				}
            } else if(account == 1) {
                if(checking > 0) {
					checking = checking - amount;
					savings = savings + amount;
                    std::cout << "Bruce transferred $" << amount
                         << " from checking to savings." << std::endl;
					printBalances();
                } else {
                    std::cout << "Bruce cannot transfer $" << amount
                         << ". Insufficient funds." << std::endl;
				}
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}
			pthread_mutex_unlock(&savingsLock);
			pthread_mutex_unlock(&checkingLock);
			pthread_mutex_unlock(&transferLock);
		}else
		{
			// Error check for input info.
            std::cout << "Not a recognized operation" << std::endl;
		}
	}
	return;
}

void mother() {
    std::string accountName;
	char operation;
	int account;
	int amount;
    std::ifstream inFile;
	inFile.open("mother.txt");
    while(!inFile.eof()) {
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
        if(account == 0) {
			accountName = "checking";
        } else if(account == 1) {
            accountName = "savings";
		}
		
        if(operation == 'd') {
			// Deposit routine for mother.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
                std::cout << "Jennifer deposited $" << amount << ". New " <<
                             accountName << " balance: $" << checking << std::endl;
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
				pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
                std::cout << "Jennifer deposited $" << amount << ". New " <<
                             accountName << " balance: $" << savings << std::endl;
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}
        } else if(operation == 'w') {
			// Withdraw routine for mother.
			// Lock whichever account is being accessed.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
                if(amount > checking) {
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
                    if(savings > 0) {
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
                        std::cout << "Jennifer withdrew $" << amount << "." << std::endl;
						printBalances();
                    } else {
                        std::cout << "Jennifer cannot withdraw $" << amount
                                  << ". Insufficient funds." << std::endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
                } else {
					checking = checking - amount;
                    std::cout << "Jennifer withdrew $" << amount << ". New "
                              << accountName << " balance: $" << checking << std::endl;
				}
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
				pthread_mutex_lock(&savingsLock);
				{
                    if(savings > 0) {
						savings = savings - amount;
                        std::cout << "Jennifer withdrew $" << amount << ". New"
                                  << accountName << " balance: $" << savings << std::endl;
                    } else {
                        std::cout << "Jennifer cannot withdraw $" << amount
                                  << ". Insufficient funds." << std::endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}	
        }else if(operation == 't') {
			// Transfer routine for mother.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
            if(account == 0) {
                if(savings > 0) {
					checking = checking + amount;
					savings = savings - amount;
                    std::cout << "Jennifer transferred $" << amount
                              << " from savings to checking." << std::endl;
					printBalances();
                } else {
                    std::cout << "Jennifer cannot transfer $" << amount
                              << ". Insufficient funds." << std::endl;
				}
            } else if(account == 1) {
                if(checking > 0) {
					checking = checking - amount;
					savings = savings + amount;
                    std::cout << "Jennifer transferred $" << amount
                              << " from checking to savings." << std::endl;
					printBalances();
                } else {
                    std::cout << "Jennifer cannot transfer $" << amount
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

void daughter()
{
    std::string accountName;
	char operation;
	int account;
	int amount;
    std::ifstream inFile("daughter.txt");
	while(!inFile.eof())
	{
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
        if(account == 0) {
			accountName = "checking";
        } else if(account == 1) {
            accountName = "savings";
		}
		
        if(operation == 'd') {
			// Deposit routine for daughter.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
                std::cout << "Jill deposited $" << amount << ". New "
                          << accountName << " balance: $" << checking << std::endl;
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
				pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
                std::cout << "Jill deposited $" << amount << ". New "
                          << accountName << " balance: $" << savings << std::endl;
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}
        } else if(operation == 'w') {
			// Withdraw routine for daughter.
			// Lock whichever account is being accessed.
            if(account == 0) {
				pthread_mutex_lock(&checkingLock);
                if(amount > checking) {
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
                    if(savings > 0) {
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
                        std::cout << "Jill withdrew $" << amount << "." << std::endl;
						printBalances();
                    } else {
                        std::cout << "Jill cannot withdraw $" << amount
                                  << ". Insufficient funds." << std::endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
                } else {
					checking = checking - amount;
                    std::cout << "Jill withdrew $" << amount << ". New "
                              << accountName << " balance: $" << checking << std::endl;
				}
				pthread_mutex_unlock(&checkingLock);
            } else if(account == 1) {
				pthread_mutex_lock(&savingsLock);
				{
                    if(savings > 0) {
						savings = savings - amount;
                        std::cout << "Jill withdrew $" << amount << ". New"
                                  << accountName << " balance: $" << savings << std::endl;
                    } else {
                        std::cout << "Jill cannot withdraw $" << amount
                                  << ". Insufficient funds." << std::endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
            } else {
				// Error check for input info.
                std::cout << "UNKNOWN ACCOUNT" << std::endl;
			}	
        } else if(operation == 't') {
			// Transfer routine for daughter.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
            if(account == 0) {
                if(savings > 0) {
					checking = checking + amount;
					savings = savings - amount;
                    std::cout << "Jill transferred $" << amount
                              << " from savings to checking." << std::endl;
					printBalances();
                } else {
                    std::cout << "Jill cannot transfer $" << amount
                              << ". Insufficient funds." << std::endl;
				}
            } else if(account == 1) {
                if(checking > 0) {
					checking = checking - amount;
					savings = savings + amount;
                    std::cout << "Jill transferred $" << amount
                              << " from checking to savings." << std::endl;
					printBalances();
                } else {
                    std::cout << "Jill cannot transfer $" << amount
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
