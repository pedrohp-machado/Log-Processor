#ifndef ANOMALYDETECTOR_HPP
#define ANOMALYDETECTOR_HPP

#include <unordered_map>
#include <string>
#include <mutex>
#include <iostream>
#include <fstream>
#include <cmath> // for abs(), sin, cos, sqrt(), atan2()
#include <iomanip> // for gettime()
#include <sstream>
#include "Transaction.hpp"
using namespace std;

// Simple anomaly detector based on user location changes within a short time frame

const double PI = 3.14159265358979323846;
const double EARTH_RADIUS_KM = 6371.0;    
const double ANOMALY_SPEED = 1000.0; // Max speed acceptable in km/h (commercial planes can reach around 900 km/h, so this is a safe threshold)

// struct to hold user last state
struct userState {
    string lastCity;
    long lastTimestamp;
};

// struct to hold coordinates (if we want to extend to geolocation-based anomaly detection)
struct coordinates {
    double lat;
    double lon;
};

class AnomalyDetector {
    private: 
        unordered_map<int, userState> userHistory; // map to hold all user states
        mutex mtx; // mutex for thread safety
        unordered_map<string, coordinates> cityCoordinates; // map to hold city coordinates for distance calculation
        
        ofstream alertFile; // file to write anomalies

        // helper to convert degrees to radians
        double toRad(double deg){
            return deg * (PI / 180.0);
        }

        // Haversine formula to calculate distance between two points on the Earth
        double haversine(const string c1, const string c2){
            if(cityCoordinates.find(c1) == cityCoordinates.end() || cityCoordinates.find(c2) == cityCoordinates.end())
                return 0.0; // If we don't have coordinates, we can't calculate distance
            
            coordinates coord1 = cityCoordinates[c1];
            coordinates coord2 = cityCoordinates[c2];

            double dLat = toRad(coord2.lat - coord1.lat);
            double dLon = toRad(coord2.lon - coord1.lon);
            double lat1 = toRad(coord1.lat);
            double lat2 = toRad(coord2.lat);

            double a = sin(dLat/2) * sin(dLat/2) + 
                       sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2);
            double c = 2 * atan2(sqrt(a), sqrt(1-a));

            return EARTH_RADIUS_KM * c; 
        }

        long parseTimestamp(const string& timestamp) {
            // Simple parser assuming format "YYYY-MM-DD HH:MM:SS"
            struct tm tm{};
            istringstream ss(timestamp);
            ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
            return mktime(&tm);
        }
    
    public:
        AnomalyDetector(){
            // For simplicity, hardcoded city coordinates (latitude and longitude)
            cityCoordinates["New York"] = {40.7128, -74.0060};
            cityCoordinates["Los Angeles"] = {34.0522, -118.2437};
            cityCoordinates["Chicago"] = {41.8781, -87.6298};
            cityCoordinates["Houston"] = {29.7604, -95.3698};
            cityCoordinates["Phoenix"] = {33.4484, -112.0740};
            cityCoordinates["Philadelphia"] = {39.9526, -75.1652};
            cityCoordinates["San Antonio"] = {29.4241, -98.4936};
            cityCoordinates["San Diego"] = {32.7157, -117.1611};
            cityCoordinates["Dallas"] = {32.7767, -96.7970};
            cityCoordinates["San Jose"] = {37.3382, -121.8863};

            alertFile.open("alerts.ndjson");
            if(!alertFile.is_open()){
                cerr << "Error opening alert file" << endl;
            } else
                cout << "AnomalyDetector initialized, ready to process transactions." << endl;
        }

        ~AnomalyDetector(){
            if(alertFile.is_open()){
                alertFile.close();
                cout << "Alerts saved to alerts.ndjson" << endl;
            }
        }

        void process(const Transaction& t){
            long currentSec = parseTimestamp(t.timestamp);

            lock_guard<mutex> lock(mtx); 

            if (userHistory.count(t.userId)){
                const auto& lastState = userHistory[t.userId];

                if(t.city != lastState.lastCity){

                    long timeDiff = labs(currentSec - lastState.lastTimestamp);

                    double timeDiffHours = timeDiff / 3600.0; // convert seconds to hours
                    double distance = haversine(lastState.lastCity, t.city); // distance in km
                    double speed = distance / timeDiffHours; // speed in km/h

                    if (speed > ANOMALY_SPEED){
                        alertFile << "{\"alert_type\": \"impossible_travel\", "
                            << "\"user_id\": " << t.userId << ", "
                            << "\"from\": \"" << lastState.lastCity << "\", "
                            << "\"to\": \"" << t.city << "\", "
                            << "\"distance_km\": " << (int)distance << ", "
                            << "\"speed_kmh\": " << (int)speed << "}" << std::endl;
                    }
                }
            }
            userHistory[t.userId] = {t.city, currentSec};
        }
};

#endif