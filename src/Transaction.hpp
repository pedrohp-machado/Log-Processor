#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

#include <string>
#include <iostream>
using namespace std;

struct Transaction {
    long id;
    int userId;
    double amount;
    string city;
    string timestamp;

    // using move to improve efficiency in data handling
    Transaction(long i, int uId, double a, string c, string ts)
        : id(i), userId(uId), amount(a), city(move(c)), timestamp(move(ts)) {}

    void print(){
        cout << "Transaction[ID: " << id 
             << ", UserID: " << userId 
             << ", Amount: " << amount 
             << ", City: " << city 
             << ", Timestamp: " << timestamp 
             << "]" << endl;
    }
};

#endif