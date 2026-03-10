#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <thread>
#include <memory>
#include <chrono>
#include "Transaction.hpp"
#include "safeQueue.hpp"
#include "anomalyDetector.hpp"

using namespace std;


// Path file
const string FILE_PATH = "data/transactions.csv";

void worker(int id, SafeQueue<unique_ptr<Transaction>>& queue, AnomalyDetector& detector) {
    unique_ptr<Transaction> trans;
    while (queue.pop(trans)) {
        detector.process(*trans);
        // Visual feedback
        if (trans->id % 200000 == 0) {
             cout << "[Worker " << id << "] Processando ID: " << trans->id << endl;
        }
    }
}

int main(int argc, char* argv[]) {

    string input_file = "data/transactions.csv";
    string output_file = "alerts.ndjson";
    int num_threads = thread::hardware_concurrency();

    // Process comand line arguments
    for(int i = 1; i < argc; ++i){
        string arg = argv[i];
        
        if(arg == "--input" && i + 1 < argc){
            input_file = argv[++i];
        } else if(arg == "--output" && i + 1 < argc){
            output_file = argv[++i];
        } else if(arg == "--threads" && i + 1 < argc){
            num_threads = stoi(argv[++i]);
        } else {
            cout << "Command usage: ./log_processor [--input <file>] [--output <file>] [--threads <num>]" << endl;
            return 0;
        }
    }

    cout << "Input file: " << input_file << endl;
    cout << "Output' file: " << output_file << endl;
    
    SafeQueue<unique_ptr<Transaction>> queue;
    AnomalyDetector detector;

    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    // Starting worker threads
    vector<thread> workers;
    cout << "Iniciando " << num_threads << " threads..." << endl;

    for (int i = 0; i < num_threads; ++i) {
        workers.emplace_back(worker, i, ref(queue), ref(detector));
    }

    // Opening file
    ifstream file(input_file);

    if (!file.is_open()) {
        cerr << "ERRO CRITICO: Nao foi possivel abrir o arquivo!" << input_file << endl;
        
        // Finishing worker threads to avoid hanging
        queue.markFinished();
        for (auto& t : workers) if (t.joinable()) t.join();
        return 1;
    }

    string line;
    
    if (!getline(file, line)) {
        cerr << "ERRO: Arquivo vazio!" << endl;
    } else {
        cout << "Cabecalho ignorado: " << line << endl;
    }

    // Reading and Parsing
    long count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string segment;
        vector<string> parts;

        while (getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() >= 5) {
            try {
                // Safe conversion
                auto t = make_unique<Transaction>(
                    stol(parts[0]), 
                    stoi(parts[1]), 
                    stod(parts[2]), 
                    parts[3], 
                    parts[4]
                );
                queue.push(move(t));
                count++;
            } catch (const exception& e) {
                cerr << "Erro de parse na linha " << count + 2 << ": " << e.what() << endl;
            }
        }
    }

    cout << "Leitura concluida! Total de linhas enviadas: " << count << endl;

    queue.markFinished();
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Tempo total de processamento: " << duration << " segundos." << endl;

    cout << "Processamento finalizado com sucesso." << endl;
    return 0;
}