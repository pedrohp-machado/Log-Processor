# Geo-Spatial Fraud Detector

A C++17 ETL engine designed to ingest and process massive financial transaction logs in real-time. The system uses the Haversine formula to calculate spatial distances and detect "impossible travel" fraud anomalies based on geographic constraints and time deltas.

## Architecture & Technical Highlights

This project avoids heavy frameworks, relying on standard C++ libraries to maximize throughput and minimize latency.

* **Concurrency:** Implements a Producer-Consumer architecture. A single producer thread handles disk I/O (CSV parsing), feeding a thread-safe queue consumed by multiple worker threads executing the business logic in parallel.
* **Thread Safety:** Custom queue implementation utilizing `std::mutex` and `std::condition_variable` to prevent race conditions and eliminate busy-waiting CPU cycles.
* **Memory Management:** Strictly adheres to RAII principles. Utilizes `std::unique_ptr` and `std::move` semantics for zero-copy data transfer between threads, preventing memory leaks and minimizing allocation overhead.
* **Geo-Spatial Mathematics:** Applies the Haversine formula to compute spherical distances between transaction coordinates, which are cached in a `std::unordered_map` for O(1) lookups.
* **Data Serialization:** Outputs detected anomalies directly to disk in NDJSON (Newline Delimited JSON) format, optimizing for downstream ingestion by log aggregators like ElasticSearch or Splunk.

## Performance Benchmarks

Tested with a localized load of 1,000,000 randomized transactions:

* **Total Execution Time:** ~2.85 seconds 
* **Throughput:** > 350,877 transactions / second
* **Hardware:** *Intel® Core™ i3 w/ 8GB ram*

## Sample Output (NDJSON)
* {"alert_type": "impossible_travel", "user_id": 382, "from": "Los Angeles", "to": "Dallas", "distance_km": 1991, "speed_kmh": 2753}

## Build and Execution

The project includes a `Makefile` configured with `-O3` and `-pthread` flags for optimized compilation.

**1. Generate the dataset:**
Creates a `transactions.csv` file with 1,000,000 rows inside the `data/` directory.
```bash
python3 generate_data.py

