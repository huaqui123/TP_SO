#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    if (clave.length() < 1){
        //No es un string valido
        return;
    }

    int index = HashMapConcurrente::hashIndex(clave);
    if (index < 0 && index > 25){
        //Si no es letra minuscula
        return;
    }

    // Checkear que onda
    // Create mutex array for each letter
    // Tenemos que hacer un array de mutex para que en cada letra tiramos lock y unlock
    
    ListaAtomica<hashMapPair> *l = tabla[index];
    hashMapPair par = std::make_pair(clave, 1);

    // Uso el this->mtxIncMax en el momento que realizamos el insertar para evitar que agregue interrumpiendo el max
    mtx[index].lock();
    if (l -> longitud() == 0){
        //while(ejecutandoMax){} // Nos genera busy waiting, usamos el mtxIncMax
        mtxIncMax.lock();
        l -> insertar(par);
        mtxIncMax.unlock();
        //mtx[index].unlock();
    } else {
        //mtx[index].unlock();
        bool exit = false;
        auto iter = l -> begin();
        for (unsigned int i = 0; i < l -> longitud(); i++) {
            if (iter.operator*().first == clave) {
                //mtx[index].lock();
                //while(ejecutandoMax){} // Nos genera busy waiting, usamos el mtxIncMax
                mtxIncMax.lock();
                iter.operator*().second += 1;
                mtxIncMax.unlock();
                //mtx[index].unlock();
                exit = true;
            }
            if (exit)
                break;
            iter.operator++();
        }
        if (!exit) {
            //mtx[index].lock();
            //while(ejecutandoMax){} // Nos genera busy waiting, usamos el mtxIncMax
            mtxIncMax.lock();
            l -> insertar(par);
            mtxIncMax.unlock();
            //mtx[index].unlock();
        }
    }
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
    int index = HashMapConcurrente::hashIndex(clave);
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
    //ejecutandoMax = true;
    this->mtxIncMax.lock();
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (auto &p : *tabla[index]) {
            if (p.second > max->second) {
                max->first = p.first;
                max->second = p.second;
            }
        }
    }
    //ejecutandoMax = false;
    this->mtxIncMax.unlock();

    return *max;
}

void fn(hashMapPair *max, std::atomic<int> *filas, std::mutex mtx){
    
}
hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    // Completar (Ejercicio 3)
    hashMapPair *max = new hashMapPair();
    max->second = 0; // Max actual

    std::vector<std::thread> threads(cant_threads);
    std::atomic<int> indice{0}; // Contador de filas terminadas

    std::mutex mtx_max;
    //ejecutandoMax = true;
    this->mtxIncMax.lock();
    for (auto &t: threads){
        // t = threads(test, &filasTerminadas, &this->mtx);
        t = std::thread([=, &indice, &mtx_max, &max] (){
            while (indice.load() < HashMapConcurrente::cantLetras) // Quedan filas por procesar
            {
                // Busco el maximo de esa fila
                int index = indice.fetch_add(1);
                
                if (index < HashMapConcurrente::cantLetras){
                    hashMapPair *maxLocal = new hashMapPair();
                    maxLocal->second = 0;
                    for (auto &p : *tabla[index]) {
                        if (p.second > maxLocal->second) {
                            maxLocal->first = p.first;
                            maxLocal->second = p.second;
                        }
                    }

                    mtx_max.lock();
                    if (maxLocal -> second > max -> second){
                        max -> first = maxLocal -> first;
                        max -> second = maxLocal -> second;
                    }
                    mtx_max.unlock();
                }
            }
            return 0;
        });
    }

    for (auto &t : threads){
        t.join();
    }
    //ejecutandoMax = false;
    this->mtxIncMax.unlock();
    return *max;
}

#endif
