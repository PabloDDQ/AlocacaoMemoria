#include <iostream> // Biblioteca Padrão do C++
#include <string> // Biblioteca Para Manipulação de Strings
#include <random> // Biblioteca Para Geração de Números Aleatórios
#include <chrono> // Medição de Tempo
#include <fstream> // Geração de Arquivos
#include <sys/resource.h> // Para medição de memória

using namespace std;
using namespace chrono;

const int MAX_REGISTROS = 1000000; // Uma constante para definir o número máximo de registros

// Dados Necessários
struct Valores {
    string nome;
    double num1;
    double num2;

    // FUNÇÃO RESPONSÁVEL PELA GERAÇÃO DE NÚMEROS ALEATÓRIOS
    void aleatorio(mt19937 &gen) {
        const string caracteres = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        uniform_int_distribution<> dist_char(0, caracteres.size() - 1);
        uniform_int_distribution<> dist_length(100, 500);

        // Geração de string aleatória com comprimento variável entre 100 e 500 caracteres
        int tamanho_nome = dist_length(gen);
        nome.resize(tamanho_nome);
        for (int i = 0; i < tamanho_nome; ++i) {
            nome[i] = caracteres[dist_char(gen)];
        }

        // GERAÇÃO DE NÚMEROS ALEATÓRIOS
        uniform_real_distribution<> dist_num(1, 10000000);
        num1 = dist_num(gen);
        num2 = dist_num(gen);
    }
};

// FUNÇÃO PARA MEDIR O USO DE MEMÓRIA
void medir_memoria(double &uso_memoria) {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    uso_memoria = usage.ru_maxrss; // em KB
}

int main() {
    auto start_time = high_resolution_clock::now(); // Tempo inicial
    mt19937 gen(static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count()));

    Valores registros[MAX_REGISTROS]; // Alocação de memória para os registros
    size_t memoria_inicial = 0;

    // Arquivo CSV para salvar os dados de execução
    ofstream dados("dados_execucao.csv");
    dados << "Registros,Tempo (s),Consumo de Memória (bytes),Memória RAM (KB)\n";

    for (int total_registros = 10000; total_registros <= MAX_REGISTROS; total_registros += 5000) {
        auto start_loop = high_resolution_clock::now(); // Tempo no início de cada iteração

        // Preenchendo registros com valores aleatórios
        for (int i = 0; i < total_registros; ++i) {
            registros[i].aleatorio(gen);
        }

        auto end_loop = high_resolution_clock::now(); // Tempo no final de cada iteração
        double tempo_execucao = duration_cast<duration<double>>(end_loop - start_loop).count(); // Calculando tempo de execução
        size_t consumo_memoria = total_registros * sizeof(Valores); // Consumo de memória

        // Medir o uso de memória RAM
        double uso_memoria;
        medir_memoria(uso_memoria);

        // Grava os dados no arquivo CSV
        dados << total_registros << "," << tempo_execucao << "," << consumo_memoria << ","
              << uso_memoria << "\n";

        // Exibe informações no console
        cout << "Registros: " << total_registros
             << ", Tempo: " << tempo_execucao << "s, Consumo de Memória: " << consumo_memoria
             << " bytes, Memória RAM: " << uso_memoria << " KB\n";

        // Endereço de memória do primeiro registro
        if (total_registros == 10000) {
            memoria_inicial = reinterpret_cast<size_t>(&registros[0]);
            cout << "Endereço de memória inicial: " << memoria_inicial << endl;
        }
    }

    // Tempo total de execução
    auto end_time = high_resolution_clock::now();
    double tempo_total_execucao = duration_cast<duration<double>>(end_time - start_time).count();
    cout << "Tempo total de execução: " << tempo_total_execucao << " segundos" << endl;

    dados.close(); // Fecha o arquivo CSV
    return 0;
}
