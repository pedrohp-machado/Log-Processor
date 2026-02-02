import csv
from datetime import datetime, timedelta
import random
from faker import Faker

fake = Faker()
NUM_LINES = 1_000_000
FILE = "logs.csv"

cities = [ "New York", "Los Angeles", "Chicago", "Houston", "Phoenix", "Philadelphia", "San Antonio", "San Diego", "Dallas", "San Jose"]

def generate_data():
    print(f"Generating {NUM_LINES} lines of log data...")
    with open(FILE, 'w', newline='') as f:
        
        writer = csv.writer(f)

        writer.writerow(["id_transaction", "user_id", "value", "city", "timestamp"]) # Write header

        start_date = (2025, 1, 1)

        for i in range(NUM_LINES):

            uId = random.randint(1, 10_000) # Random user ID between 1 and 10,000
            time = start_date + timedelta(seconds=random.randint(0, 31_536_000))  # Random time in 2025

            writer.writerow([
                i, 
                uId, 
                round(random.uniform(10.0, 1000000.0), 2), # Random value between 10.0 and 1,000,000.0
                random.choice(cities), # Random city from list
                datetime.strftime(time, "%Y-%m-%d %H:%M:%S")
            ])
    print(f"Log data generation completed. Data saved to {FILE}.")


if __name__ == "__main__":
    generate_data()