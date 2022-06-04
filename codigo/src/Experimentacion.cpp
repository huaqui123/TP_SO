#include <iostream>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>

#include "HashMapConcurrente.hpp"
#include "CargarArchivos.hpp"

using namespace std;

#define BILLION  1000000000L;

double tiempoElapsado;
struct timespec start, stop;
vector<string> filePaths = {};

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

void calcularTiempo(int threads, struct timespec start, struct timespec stop){
    if (stop.tv_nsec < start.tv_nsec) {
        stop.tv_nsec += 1000000000;
        stop.tv_sec--;
    }
    std::cout << "Tiempo elapsado " << threads << " threads" << std::endl;
    printf("%ld.%09ld\n", (long)(stop.tv_sec - start.tv_sec),
        stop.tv_nsec - start.tv_nsec);
}

HashMapConcurrente correrCargarArchivosConNThreads(int threads){
    HashMapConcurrente hashMap1{}; // = HashMapConcurrente();
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    cargarMultiplesArchivos(hashMap1, threads, filePaths);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    calcularTiempo(threads, start, stop);
    return hashMap1;
}

void correrMaximoParaleloConNThreads(int threads, HashMapConcurrente hashMap1){
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    auto maximo = hashMap1.maximoParalelo(threads);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    std::cout << maximo.first << " " << maximo.second << std::endl;
    calcularTiempo(threads, start, stop);
}

void experimento1(){
    // Experimento 1
    // Con el doble de threads deberia tardar la mitad de tiempo
    
    HashMapConcurrente hashMap;
    
    filePaths.push_back("../archivos/exp1.txt");
    filePaths.push_back("../archivos/exp2.txt");
    filePaths.push_back("../archivos/exp3.txt");
    filePaths.push_back("../archivos/exp4.txt");
    filePaths.push_back("../archivos/exp5.txt");
    filePaths.push_back("../archivos/exp6.txt");
    filePaths.push_back("../archivos/exp7.txt");
    filePaths.push_back("../archivos/exp8.txt");

    // Cargar archivos con 1 thread
    hashMap = correrCargarArchivosConNThreads(1);

    // Calcular maximo con 1 thread
    correrMaximoParaleloConNThreads(1, hashMap);

    // Cargar archivos con 8 threads
    hashMap = correrCargarArchivosConNThreads(8);

    // Calcular maximo con 8 threads
    correrMaximoParaleloConNThreads(8, hashMap);
}

int main(int argc, char **argv) {
    cout << "Ejecutando experimento 1" << endl;
    experimento1();
    return 0;
}
