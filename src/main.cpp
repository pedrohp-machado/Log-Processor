#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <memory>
#include "safeQueue.hpp"
#include "Transaction.hpp"
using namespace std;

// Function for worker threads to process transactions
void worker(int id, SafeQueue<unique_ptr<Transaction>>& queue) {
    unique_ptr<Transaction> trans;
    while(queue.pop(trans)){
        // Only going up to 100k for demonstration
        if (trans->id % 1000000 == 0)
            cout << "Worker " << id << " processing transaction ID: " << trans->id << endl;
        
        // since it is a unique_ptr, it is not necessary to manually delete
    }
}

int main(){

    SafeQueue<unique_ptr<Transaction>> queue; 

    // starting workers
    int num_threads = thread::hardware_concurrency();
    vector<thread> threads;

    for(int i = 0; i < num_threads; ++i){
        threads.emplace_back(worker, i, ref(queue));
    }

    ifstream file("data/transactions.csv");
    string line;
    getline(file, line); // skip header

    cout << "Starting data ingestions... " << endl;
    while(getline(file, line)){
        stringstream ss(line);
        string segment;
        vector<string> segList;

        while(getline(ss, segment, ','))
            segList.push_back(segment);
        

        // parsing transaction and adding to queue 
        if(segList.size() >= 5){
            auto t = make_unique<Transaction>(
                stol(segList[0]),
                stol(segList[1]),
                stol(segList[2]),
                segList[3],
                segList[4]
            );
            queue.push(move(t));
        } 
    }

    queue.markFinished(); // signal no more data

    for(auto& t : threads)
        if (t.joinable()) t.join();

    cout << "All transactions processed." << endl;
    return 0;
}
