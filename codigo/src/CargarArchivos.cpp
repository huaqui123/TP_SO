#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <pthread.h>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    // Completar (Ejercicio 4)
    
    std::vector<std::thread> threads(cantThreads);

    std::atomic<int> resto{filePaths.size() % cantThreads};
    int cant_files_per_thread = filePaths.size() / cantThreads;

    int i = 0;
    while (i < threads.size()){
        threads[i] = std::thread([=, &hashMap, &resto] (){
            int j = 0;
            while (j < cant_files_per_thread) {
                cargarArchivo(hashMap, filePaths[i * cant_files_per_thread + j]);
                j++;
            }
            j = 0;
            while(resto.load() > 0) {
                int index = resto.fetch_add(-1);
                if (index > 0){
                    cargarArchivo(hashMap, filePaths[cant_files_per_thread * cantThreads + index - 1]);
                }
            }
            std::cout << "Thread "<< i << " a terminado" << std::endl;
            return 0;
        });
        i++;
    }

    for (auto &t : threads){
        t.join();
    }

}

#endif
