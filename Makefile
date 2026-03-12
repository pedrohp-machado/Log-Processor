CXX = g++

# Flags
# -pthread: enable multithreading
CXXFLAGS = -std=c++17 -pthread -O3 -Wall

TARGET = log_processor

SRC = src/main.cpp

all: $(TARGET)

# Como compilar o executável
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
	@echo "Build successful! executable: ./$(TARGET)"

run: all
	@echo "Running processor..."
	./$(TARGET)

clean:
	rm -f $(TARGET)
	@echo "Limpeza concluída."

# Tests
TEST_TARGET = log_processor_test
TEST_SRC = src/tests/tests.cpp

test: $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)
	@echo "Test build successful: executable: ./$(TEST_TARGET)"
	./$(TEST_TARGET)


# Sanitizer 
sanitize_mem:
	@echo "Building w/ memory sanitizer..."
	$(CXX) $(CXXFLAGS) -fsanitize=address -g $(SRC) -o log_processor_mem
	@echo "Running w/ memory sanitizer..."
	./log_processor_mem --input data/transactions.csv
	@echo "Sanitizer run complete."

sanitize_thread:
	@echo "Building w/ thread sanitizer..."
	$(CXX) $(CXXFLAGS) -fsanitize=thread -g $(SRC) -o log_processor_thread
	@echo "Running w/ thread sanitizer..."
	setarch x86_64 -R ./log_processor_thread --input data/transactions.csv
	@echo "Sanitizer run complete."