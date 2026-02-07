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


// Caminho do arquivo (ajuste se necessário)
const std::string FILE_PATH = "data/transactions.csv";

void worker(int id, SafeQueue<std::unique_ptr<Transaction>>& queue, AnomalyDetector& detector) {
    std::unique_ptr<Transaction> trans;
    while (queue.pop(trans)) {
        detector.process(*trans);
        // Feedback visual a cada 200 mil linhas para não poluir
        if (trans->id % 200000 == 0) {
             std::cout << "[Worker " << id << "] Processando ID: " << trans->id << std::endl;
        }
    }
}

int main() {
    
    SafeQueue<std::unique_ptr<Transaction>> queue;
    AnomalyDetector detector;

    chrono::steady_clock::time_point start = chrono::steady_clock::now();

    // 1. Inicia os Workers
    std::vector<std::thread> workers;
    int num_threads = std::thread::hardware_concurrency();
    std::cout << "Iniciando " << num_threads << " threads..." << std::endl;

    for (int i = 0; i < num_threads; ++i) {
        workers.emplace_back(worker, i, std::ref(queue), std::ref(detector));
    }

    // 2. Abre o Arquivo
    std::cout << "Tentando abrir arquivo em: " << FILE_PATH << std::endl;
    std::ifstream file(FILE_PATH);

    if (!file.is_open()) {
        std::cerr << "ERRO CRITICO: Nao foi possivel abrir o arquivo!" << std::endl;
        std::cerr << "Verifique se a pasta 'data' existe e se o arquivo 'transactions.csv' esta dentro dela." << std::endl;
        
        // Encerra as threads e sai
        queue.markFinished();
        for (auto& t : workers) if (t.joinable()) t.join();
        return 1;
    }

    std::string line;
    
    // 3. Pula o cabeçalho (IMPORTANTE)
    if (!std::getline(file, line)) {
        std::cerr << "ERRO: Arquivo vazio!" << std::endl;
    } else {
        std::cout << "Cabecalho ignorado: " << line << std::endl;
    }

    // 4. Leitura e Parsing
    long count = 0;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string segment;
        std::vector<std::string> parts;

        while (std::getline(ss, segment, ',')) {
            parts.push_back(segment);
        }

        if (parts.size() >= 5) {
            try {
                // Conversão segura
                auto t = std::make_unique<Transaction>(
                    std::stol(parts[0]), 
                    std::stoi(parts[1]), 
                    std::stod(parts[2]), 
                    parts[3], 
                    parts[4]
                );
                queue.push(std::move(t));
                count++;
            } catch (const std::exception& e) {
                // Se der erro numa linha, avisa mas não para
                std::cerr << "Erro de parse na linha " << count + 2 << ": " << e.what() << std::endl;
            }
        }
    }

    std::cout << "Leitura concluida! Total de linhas enviadas: " << count << std::endl;

    // 5. Finalização
    queue.markFinished();
    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    chrono::steady_clock::time_point end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start).count();
    cout << "Tempo total de processamento: " << duration << " segundos." << endl;

    cout << "Processamento finalizado com sucesso." << std::endl;
    return 0;
}