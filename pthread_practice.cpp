//  Name: Joshua McCarville-Schueths
//	File: pthread_practice.cpp
//  Description: This program demonstrates creating threads,
//							 passing data to threads, and using mutual
//							 exclusion to prevent concurrency issues. The
//							 way the program does that is by simulating
//         			 a shared set of savings and checking accounts
//							 for a three member family.


#include "pthread_practice.h"

using namespace std;

int main(int argc, char *argv[])
{	
	// Declare thread id's and filename strings.
	pthread_t tid[3];
	string file[3];
	
	// Make sure there are enough files entered.
	if (argc != 4)
	{
		cout << "Please make sure you have entered: " << endl;
		cout << "father.txt" << endl << "mother.txt" << endl;
		cout << "daughter.txt" << endl;
	}else
	{
		// Create 3 threads and pass the filenames to each one.
		for (int i=1; i < 4; i++)
		{
			file[i-1] = argv[i];
			pthread_create(&tid[i-1], NULL, family_member, (void *) &file[i-1]);
		}
		for (int i=0; i < 3; i++)
		{
			// Have each thread rejoin after its execution.
			pthread_join(tid[i], NULL);
		}
	}
	return 0;
}

// Thread that simulates a family member.
void *family_member(void * text_file)
{
	string *file_name;
	file_name = (string *) text_file;
	if (*(file_name) == "father.txt")
	{
		// If passed the father file, simulate the father.
		father();
	}else
	if (*(file_name) == "mother.txt")
	{
		// If passed the mother file, simulate the mother.
		mother();
	}else
	if (*(file_name) == "daughter.txt")
	{
		// If passed the daughter file, simulate the daughter.
		daughter();
	}
	return 0;
}

// Function that simulates the father.
void father()
{
	string accountName;
	char operation;
	int account;
	int amount;
	ifstream inFile;
	inFile.open("father.txt");
	while(!inFile.eof())
	{
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
		if (account == 0) // This block interprets the number
		{									// into english for output.
			accountName = "checking";
		}else
		if (account == 1)
		{
			accountName == "savings";
		}
		
		if (operation == 'd')
		{
			// Deposit routine for father.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
				cout << "Bruce deposited $" << amount << ". New " <<
					accountName << " balance: $" << checking << endl;
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
				cout << "Bruce deposited $" << amount << ". New " <<
					accountName << " balance: $" << savings << endl;
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
		}else
		if (operation == 'w')
		{
			// Withdraw routine for father.
			// Lock whichever account is being accessed.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				if (amount > checking)
				{
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
					if (savings > 0)
					{
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
						cout << "Bruce withdrew $" << amount << "." << endl;
						printBalances();
					}else
					{
						cout << "Bruce cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
				}else
				{
					checking = checking - amount;
					cout << "Bruce withdrew $" << amount << ". New " <<
						accountName << " balance: $" << checking << endl;
				}
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				{
					if (savings > 0)
					{
						savings = savings - amount;
						cout << "Bruce withdrew $" << amount << ". New" <<
							accountName << " balance: $" << savings << endl;
					}else
					{
						cout << "Bruce cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}	
		}else
		if (operation == 't')
		{
			// Transfer routine for father.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
			if (account == 0)
			{
				if (savings > 0)
				{
					checking = checking + amount;
					savings = savings - amount;
					cout << "Bruce transferred $" << amount <<
						" from savings to checking." << endl;
					printBalances();
				}else
				{
					cout << "Bruce cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			if (account == 1)
			{
				if (checking > 0)
				{
					checking = checking - amount;
					savings = savings + amount;
					cout << "Bruce transferred $" << amount <<
						" from checking to savings." << endl;
					printBalances();
				}else
				{
					cout << "Bruce cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
			pthread_mutex_unlock(&savingsLock);
			pthread_mutex_unlock(&checkingLock);
			pthread_mutex_unlock(&transferLock);
		}else
		{
			// Error check for input info.
			cout << "Not a recognized operation" << endl;
		}
	}
	inFile.close();
	return;
}

void mother()
{
	string accountName;
	char operation;
	int account;
	int amount;
	ifstream inFile;
	inFile.open("mother.txt");
	while(!inFile.eof())
	{
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
		if (account == 0)
		{
			accountName = "checking";
		}else
		if (account == 1)
		{
			accountName == "savings";
		}
		
		if (operation == 'd')
		{
			// Deposit routine for mother.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
				cout << "Jennifer deposited $" << amount << ". New " <<
					accountName << " balance: $" << checking << endl;
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
				cout << "Jennifer deposited $" << amount << ". New " <<
					accountName << " balance: $" << savings << endl;
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
		}else
		if (operation == 'w')
		{
			// Withdraw routine for mother.
			// Lock whichever account is being accessed.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				if (amount > checking)
				{
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
					if (savings > 0)
					{
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
						cout << "Jennifer withdrew $" << amount << "." << endl;
						printBalances();
					}else
					{
						cout << "Jennifer cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
				}else
				{
					checking = checking - amount;
					cout << "Jennifer withdrew $" << amount << ". New " <<
						accountName << " balance: $" << checking << endl;
				}
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				{
					if (savings > 0)
					{
						savings = savings - amount;
						cout << "Jennifer withdrew $" << amount << ". New" <<
							accountName << " balance: $" << savings << endl;
					}else
					{
						cout << "Jennifer cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}	
		}else
		if (operation == 't')
		{
			// Transfer routine for mother.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
			if (account == 0)
			{
				if (savings > 0)
				{
					checking = checking + amount;
					savings = savings - amount;
					cout << "Jennifer transferred $" << amount <<
						" from savings to checking." << endl;
					printBalances();
				}else
				{
					cout << "Jennifer cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			if (account == 1)
			{
				if (checking > 0)
				{
					checking = checking - amount;
					savings = savings + amount;
					cout << "Jennifer transferred $" << amount <<
						" from checking to savings." << endl;
					printBalances();
				}else
				{
					cout << "Jennifer cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
			pthread_mutex_unlock(&savingsLock);
			pthread_mutex_unlock(&checkingLock);
			pthread_mutex_unlock(&transferLock);
		}else
		{
			// Error check for input info.
			cout << "Not a recognized operation" << endl;
		}
	}
	inFile.close();
	return;
}

void daughter()
{
	string accountName;
	char operation;
	int account;
	int amount;
	ifstream inFile;
	inFile.open("daughter.txt");
	while(!inFile.eof())
	{
		inFile >> operation;
		inFile >> account;
		inFile >> amount;
		if (account == 0)
		{
			accountName = "checking";
		}else
		if (account == 1)
		{
			accountName == "savings";
		}
		
		if (operation == 'd')
		{
			// Deposit routine for daughter.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				checking = checking + amount;
				cout << "Jill deposited $" << amount << ". New " <<
					accountName << " balance: $" << checking << endl;
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				savings = savings + amount;
				cout << "Jill deposited $" << amount << ". New " <<
					accountName << " balance: $" << savings << endl;
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
		}else
		if (operation == 'w')
		{
			// Withdraw routine for daughter.
			// Lock whichever account is being accessed.
			if (account == 0)
			{
				pthread_mutex_lock(&checkingLock);
				if (amount > checking)
				{
					// Lock both accounts if transfering.
					pthread_mutex_lock(&transferLock);
					pthread_mutex_lock(&savingsLock);
					if (savings > 0)
					{
						savings = savings + checking;
						checking = 0;
						savings = savings - amount;
						cout << "Jill withdrew $" << amount << "." << endl;
						printBalances();
					}else
					{
						cout << "Jill cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
					pthread_mutex_unlock(&savingsLock);
					pthread_mutex_unlock(&transferLock);
				}else
				{
					checking = checking - amount;
					cout << "Jill withdrew $" << amount << ". New " <<
						accountName << " balance: $" << checking << endl;
				}
				pthread_mutex_unlock(&checkingLock);
			}else
			if (account == 1)
			{
				pthread_mutex_lock(&savingsLock);
				{
					if (savings > 0)
					{
						savings = savings - amount;
						cout << "Jill withdrew $" << amount << ". New" <<
							accountName << " balance: $" << savings << endl;
					}else
					{
						cout << "Jill cannot withdraw $" << amount <<
							". Insufficient funds." << endl;
					}
				}
				pthread_mutex_unlock(&savingsLock);
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}	
		}else
		if (operation == 't')
		{
			// Transfer routine for daughter.
			// Lock both accounts when transfering.
			pthread_mutex_lock(&transferLock);
			pthread_mutex_lock(&checkingLock);
			pthread_mutex_lock(&savingsLock);
			if (account == 0)
			{
				if (savings > 0)
				{
					checking = checking + amount;
					savings = savings - amount;
					cout << "Jill transferred $" << amount <<
						" from savings to checking." << endl;
					printBalances();
				}else
				{
					cout << "Jill cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			if (account == 1)
			{
				if (checking > 0)
				{
					checking = checking - amount;
					savings = savings + amount;
					cout << "Jill transferred $" << amount <<
						" from checking to savings." << endl;
					printBalances();
				}else
				{
					cout << "Jill cannot transfer $" << amount <<
						". Insufficient funds." << endl;
				}
			}else
			{
				// Error check for input info.
				cout << "UNKNOWN ACCOUNT" << endl;
			}
			pthread_mutex_unlock(&savingsLock);
			pthread_mutex_unlock(&checkingLock);
			pthread_mutex_unlock(&transferLock);
		}else
		{
			// Error check for input info.
			cout << "Not a recognized operation" << endl;
		}
	}
	inFile.close();
	return;
}

// Prints the balances of both accounts.
// Used when both accounts have been affected
// during a transaction.
void printBalances()
{
	cout << "Checking Balance: $" << checking << endl;
	cout << "Savings Balance: $" << savings << endl;
	return;
}
