#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

std::mutex mtxIncMax;
std::mutex mtx[HashMapConcurrente::cantLetras]; // Chequear

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int getIndex(std::string clave){
    return (int)clave[0] - 97;
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    if (clave.length() < 1){
        //No es un string valido
        return;
    }

    int index = getIndex(clave);
    if (index < 0 && index > 25){
        //Si no es letra minuscula
        return;
    }

    // Checkear que onda
    // Create mutex array for each letter
    // Tenemos que hacer un array de mutex para que en cada letra tiramos lock y unlock
    mtx[index].lock();
    //mtxIncMax.lock();
    ListaAtomica<hashMapPair> *l = tabla[index];
    hashMapPair par = std::make_pair(clave, 1);
    if (l -> longitud() == 0){
        l -> insertar(par);
    } else {
        bool exit = false;
        auto iter = l -> begin();
        for (unsigned int i = 0; i < l -> longitud(); i++) {
            if (iter.operator*().first == clave) {
                iter.operator*().second += 1;
                exit = true;
            }
            if (exit)
                break;
            iter.operator++();
        }
        if (!exit) {
            l -> insertar(par);
        }
    }
    //mtxIncMax.unlock();
    mtx[index].unlock();
    return;

}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> *res = new std::vector<std::string>();

    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        auto iter = tabla[i] -> begin();
        for (unsigned int j = 0; j < tabla[i] -> longitud(); j++) {
            res -> push_back(iter.operator*().first);
            iter.operator++();
        }
    }
    return *res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int index = getIndex(clave);
    auto iter = tabla[index] -> begin();
    for (unsigned int i = 0; i < tabla[index] -> longitud(); i++) {
        if (iter.operator*().first == clave) {
            return iter.operator*().second;
        }
        iter.operator++();
    }
    return 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair *max = new hashMapPair();
    max->second = 0;

    // Para que no haya inconsistencias en caso de que maximo
    // se ejecuta concurrentemente con incrementar, creamos un 
    // mutex que se usa en estas dos funciones.
    mtxIncMax.lock();
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }
    mtxIncMax.unlock();

    return *max;
}

void fn(hashMapPair *max, std::atomic<int> *filas, std::mutex *mtx){
    
}
hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    // Completar (Ejercicio 3)
    hashMapPair *max = new hashMapPair();
    max->second = 0; // Max actual

    std::atomic<int> filasTerminadas{0}; // Contador de filas terminadas
    std::mutex mtx;

    std::vector<std::thread> threads(cant_threads);

    for (auto &t: threads){
        // t = threads(test, &filasTerminadas, &mtx);
        t = std::thread([=, &filasTerminadas, &mtx, &max] (){
            while (filasTerminadas.load() < HashMapConcurrente::cantLetras) // Quedan filas por procesar
            {
                // Busco el maximo de esa fila
                // mtx -> lock();
                //int index = filasTerminadas.load();
                // mtx -> unlock();
                filasTerminadas.fetch_add(1);

                hashMapPair *maxLocal = new hashMapPair();
                maxLocal->second = 0;
                // for (auto &p : tabla[index]) {
                //     if (p.second > maxLocal->second) {
                //         maxLocal->first = p.first;
                //         maxLocal->second = p.second;
                //     }
                // }

                mtx.lock();
                // if (maxLocal -> second > max -> second){
                //     max -> first = maxLocal -> first;
                //     max -> second = maxLocal -> second;
                // }
                mtx.unlock();
            }

            return 0;
        });
    }

    for (auto &t : threads){
        t.join();
    }

    return *max;
}

#endif
