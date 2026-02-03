#ifndef SAFEQUEUE_HPP
#define SAFEQUEUE_HPP

#include <queue> 
#include <mutex>
#include <condition_variable>
#include <memory>
using namespace std;

template <typename T>
class SafeQueue { 
    private: 
        queue<T> q; // queue to store data 
        mutex mtx; // mutex for synchronizing access to the queue
        condition_variable cv; // condition variable for signaling
        bool finished = false; // Flag to indicate if there is more data to be added

    public:

        // Function to add item to the queue
        void push(T item){
            lock_guard<mutex> lock(mtx); // lock the mutex to ensure thread safety
            
            // add item to queue
            q.push(move(item)); // using move to not copy the item
            
            cv.notify_one(); // notify one waiting thread that new data is available 
        }

        // Function to retrieve item from the queue
        bool pop(T& item){
            
            // unique lock to manually control locking and unlocking
            unique_lock<mutex> lock(mtx); // unique_lock is similar to X-Lock or Write lock

            // unlock mutex and wait until notified or finished 
            cv.wait(lock, [this], { return !q.empty() || finished; });

            if(q.empty() && finished) return false; // indicate no more data 

            item = move(q.front()); // 
            q.pop(); // remove item
            
            return true; // indicate retrieval
        }

        void markFinished(){
            lock_guard<mutex> lock(mtx); // lock the mutex
            finished = true; // set the finished flag
            cv.notify_all(); // notify waiting threads;
        }

}

#endif