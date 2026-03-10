#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../anomalyDetector.hpp"

TEST_CASE("Haversine Distance Calculation"){
    AnomalyDetector detector;

    SUBCASE("Distance between same cities should be zero"){
        CHECK(detector.haversine("New York", "New York") == 0.0);
    }

    SUBCASE("Distance non existent cities should be zero"){
        CHECK(detector.haversine("Metropolis", "Gotham") == 0.0);
    }

    SUBCASE("Distance between NY and LA should be around 3935 km"){
        double dist = detector.haversine("New York", "Los Angeles");
        CHECK(dist > 3900.0);
        CHECK(dist < 4000.0);
    }
}