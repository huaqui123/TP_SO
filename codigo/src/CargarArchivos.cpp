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

    /* std::atomic<int> resto{filePaths.size() % cantThreads};
    int cant_files_per_thread = filePaths.size() / cantThreads;

    int i = 0;
    while (i < threads.size()){
        Draf de idea
        Leemos el archivo como en cargarArchivo y va agarrando c/u
        Cuando hay que cambiar de archivo tiramos un lock en la variable del archivo y el encargado se encarga de cambiar

        
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
            //std::cout << "Thread "<< i << " a terminado" << std::endl;
            return 0;
        });
        i++;
    } */
    int i = 0;
    std::mutex mtx_file;
    std::fstream file;
    file.open(filePaths[0], file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePaths[0] << "'" << std::endl;
    }
    std::atomic<int> nroArchivo{1};
    std::atomic<int> a{1};
    while (i < threads.size()){
        threads[i] = std::thread([=, &hashMap, &mtx_file, &nroArchivo, &file, &filePaths, &a] (){
        std::string palabraActual;
        while(nroArchivo.load() - 1 < filePaths.size()){
            while (file >> palabraActual) {
                hashMap.incrementar(palabraActual);
            }
            mtx_file.lock();
            if (file.eof()) {
                file.close();
                int index = nroArchivo.fetch_add(1);
                if (index < filePaths.size()){
                    file.clear();
                    file.open(filePaths[index], file.in);
                }
            }
            mtx_file.unlock();
        }

        });
        i++;
    }
    for (auto &t : threads){
        t.join();
    }

}

#endif
