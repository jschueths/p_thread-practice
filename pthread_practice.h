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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// Thread that is allows parallel processing.
void *family_member(void * text_file);
// Simulates the father's actions.
void father();
// Simulates the mother's actions.
void mother();
// Simulates the daughter's actions.
void daughter();
// Prints the current account balances.
void printBalances();

// Global Variables for Threads //
int savings = 0; // Savings account value.
int checking = 0; // Checking account value.
pthread_mutex_t savingsLock; // Mutex for savings.
pthread_mutex_t checkingLock; // Mutex for checking.
pthread_mutex_t transferLock; // Mutex to prevent transfer deadlock.
