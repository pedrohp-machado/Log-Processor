#ifndef ANOMALYDETECTOR_HPP
#define ANOMALYDETECTOR_HPP

#include <unordered_map>
#include <string>
#include <mutex>
#include <iostream>
#include <cmath> // for abs()
#include <iomanip> // for gettime()
#include <sstream>
#include "Transaction.hpp"
using namespace std;

// struct to hold user last state
struct userState {
    string lastCity;
    long lastTimestamp;
};

class AnomalyDetector {
    private: 
        unordered_map<int, userState> userHistory; // map to hold all user states
        mutex mtx; // mutex for thread safety


        long parseTimestamp(const string& timestamp) {
            // Simple parser assuming format "YYYY-MM-DD HH:MM:SS"
            struct tm tm{};
            istringstream ss(timestamp);
            ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
            return mktime(&tm);
        }
    
    public:
        void process(const Transaction& t){
            long currentSec = parseTimestamp(t.timestamp);

            lock_guard<mutex> lock(mtx); 

            if (userHistory.count(t.userId)){
                const auto& lastState = userHistory[t.userId];

                // For simplicity, consider anomaly if city changed within 1 hour
                long timeDiff = labs(currentSec - lastState.lastTimestamp);
                if(t.city != lastState.lastCity && timeDiff < 3600){
                    cout << "Anomaly detected for UserID: " << t.userId
                         <<  " | Previous City: " << lastState.lastCity
                         <<  " | Curret City: " << t.city
                         <<  " | Time difference: " << timeDiff << "s" << endl;
                }
            }
            userHistory[t.userId] = {t.city, currentSec};
        }
};

#endif