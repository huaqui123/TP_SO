#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
// alternativamente #include <pthread.h>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

std::mutex mtxIncMax;

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

    // FALTA
    // Create mutex array for each letter
    std::mutex mtx[HashMapConcurrente::cantLetras]; // Chequear

    // Tenemos que hacer un array de mutex para que en cada letra tiramos lock y unlock
    mtx[index].lock();
    mtxIncMax.lock();
    ListaAtomica<hashMapPair> *l = tabla[index];
    hashMapPair par = std::make_pair(clave, 1);
    if (l -> longitud() == 0){
        l -> insertar(par);
    } else {
        bool exit = false;
        auto iter = l -> begin();
        while (&iter.operator++ != nullptr) {
            if (&(iter.operator*) == clave) {
                iter._nodo_sig += 1;
                exit = true;
            }
            if (exit)
                break;
            iter.operator++;
        }
        if (!exit) {
            l -> insertar(par);
        }
    }
    mtxIncMax.unlock();
    mtx[index].unlock();
    return;

}

std::vector<std::string> HashMapConcurrente::claves() {
    std::vector<std::string> res = new std::vector<std::string>();
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        auto iter = tabla[i].begin();
        while (iter != nullptr) {
            res.push(iter*.first == clave);
            iter++;
        }
    }
    return res;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    int res = -1;
    int index = getInted(clave);
    auto iter = tabla[index].begin();
    while (iter != nullptr) {
        if (iter*.first == clave) {
            return iter*.second;
        }
        iter++;
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



hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cant_threads) {
    // Completar (Ejercicio 3)
    hashMapPair *max = new hashMapPair();
    return *max;
}

#endif
