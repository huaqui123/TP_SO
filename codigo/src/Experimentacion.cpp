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

double calcularTiempo(string msg, struct timespec start, struct timespec stop){
    if (stop.tv_nsec < start.tv_nsec) {
        stop.tv_nsec += 1000000000;
        stop.tv_sec--;
    }
    std::cout << msg << "\n";
    printf("%ld.%09ld\n", (long)(stop.tv_sec - start.tv_sec),
        stop.tv_nsec - start.tv_nsec);
    // return  ( stop.tv_sec - start.tv_sec ) 
    //         + ( stop.tv_nsec - start.tv_nsec )
    //         / 1.0e6;
}

void experimento1(){
    // Experimento 1
    // Con el doble de threads deberia tardar la mitad de tiempo
    double tiempoElapsado;
    struct timespec start, stop;
    vector<string> filePaths = {};

    filePaths.push_back("../archivos/exp1.txt");
    filePaths.push_back("../archivos/exp2.txt");
    filePaths.push_back("../archivos/exp3.txt");
    filePaths.push_back("../archivos/exp4.txt");
    filePaths.push_back("../archivos/exp5.txt");
    filePaths.push_back("../archivos/exp6.txt");
    filePaths.push_back("../archivos/exp7.txt");
    filePaths.push_back("../archivos/exp8.txt");

    // Cargar archivos 1 thread
    HashMapConcurrente hashMap1{}; // = HashMapConcurrente();
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    cargarMultiplesArchivos(hashMap1, 1, filePaths);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    calcularTiempo("Tiempo elapsado 1 thread:", start, stop);

    // Calcular maximo 1 thread
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    auto maximo = hashMap1.maximoParalelo(1);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    std::cout << maximo.first << " " << maximo.second << std::endl;
    calcularTiempo("Tiempo elapsado 1 thread:", start, stop);

    HashMapConcurrente hashMap{}; // = HashMapConcurrente();
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    cargarMultiplesArchivos(hashMap, 8, filePaths);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    calcularTiempo("Tiempo elapsado 8 threads:", start, stop);

    // Calcular maximo 8 threads
    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    maximo = hashMap.maximoParalelo(8);
    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
      perror( "clock gettime" );
      exit( EXIT_FAILURE );
    }
    std::cout << maximo.first << " " << maximo.second << std::endl;
    calcularTiempo("Tiempo elapsado 1 thread:", start, stop);
}

int main(int argc, char **argv) {
    cout << "Ejecutando experimento 1" << endl;
    experimento1();
    return 0;
}
