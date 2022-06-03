#include <iostream>
#include <ctime>
#include <fstream>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

using namespace std;

void crearArchivos(){
    ofstream file1("../archivos/exp1.txt");
    for(int i = 0; i < 1000000; ++i){
        file1 << "hola ";
    }
    file1.close();

    ofstream file2("../archivos/exp2.txt");
    for(int i = 0; i < 1000000; ++i){
        file2 << "hola ";
    }
    file2.close();
}

void experimento1(){
    // Experimento 1
    // Con el doble de threads deberia tardar la mitad de tiempo
    // crearArchivos();

    clock_t start;
    double duration;
    vector<string> filePaths = {};
    filePaths.push_back("../archivos/exp1.txt");
    filePaths.push_back("../archivos/exp2.txt");
    filePaths.push_back("../archivos/exp3.txt");
    filePaths.push_back("../archivos/exp4.txt");

    HashMapConcurrente hashMap{}; // = HashMapConcurrente();

    // start = clock();
    // cargarMultiplesArchivos(hashMap, 1, filePaths);
    // duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout << "Cargar multiples archivos con 1 thread tuvo una duracion de: " << duration << endl;

    // start = clock();
    // auto maximo = hashMap.maximoParalelo(1);
    // duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    // cout << "Calcular el maximo con 1 thread tuvo una duracion de: " << duration << endl;
    // cout << "Maximo: " << maximo.second << endl;

    start = clock();
    cargarMultiplesArchivos(hashMap, 4, filePaths);
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Cargar multiples archivos con 2 thread tuvo una duracion de: " << duration << endl;

    start = clock();
    auto maximo = hashMap.maximoParalelo(2);
    duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
    cout << "Calcular el maximo con 2 thread tuvo una duracion de: " << duration << endl;
    cout << "Maximo: " << maximo.second << endl;
}

int main(int argc, char **argv) {
    cout << "Ejecutando experimento 1" << endl;
    experimento1();
    return 0;
}
