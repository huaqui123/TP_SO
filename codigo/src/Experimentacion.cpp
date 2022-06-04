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

vector<vector<string>> tiemposEjecutados = {};

void crearArchivos(){
    ofstream file1("./archivos/exp1.txt");
    for(int i = 0; i < 1000000; ++i){
        file1 << "hola ";
    }
    file1.close();

    ofstream file2("./archivos/exp2.txt");
    for(int i = 0; i < 1000000; ++i){
        file2 << "hola ";
    }
    file2.close();
}

void calcularTiempo(int threads, struct timespec start, struct timespec stop, string origen){
    if (stop.tv_nsec < start.tv_nsec) {
        stop.tv_nsec += 1000000000;
        stop.tv_sec--;
    }
    std::cout << "Tiempo elapsado con " << threads << " threads ejecutando " << origen << std::endl;
    printf("%ld.%09ld\n", (long)(stop.tv_sec - start.tv_sec),
        stop.tv_nsec - start.tv_nsec);
    ofstream myfile;
    myfile.open ("./archivos/muestra.csv", std::ios_base::app | std::ios_base::out);
    myfile << origen + "," + std::to_string(threads) + "," + std::to_string((long)(stop.tv_sec - start.tv_sec)) + "." +
        std::to_string(stop.tv_nsec - start.tv_nsec) + "\n";
    myfile.close();
}

HashMapConcurrente correrCargarArchivosConNThreads(int threads ){//, &HashMapConcurrente hasMap1){
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
    calcularTiempo(threads, start, stop, "Carga");
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
    calcularTiempo(threads, start, stop, "Máximo");
}

void experimento1(){
    // Experimento 1
    // Con el doble de threads deberia tardar la mitad de tiempo
    cout << "Entramos experimento 1" << endl;

    int max_threads = 8;
    for (int i = 0; i < max_threads; i++) {
        tiemposEjecutados.push_back(vector<string>{});
        tiemposEjecutados[i].push_back("");
        tiemposEjecutados[i].push_back("");
    }
    
    HashMapConcurrente hashMap;
    
    filePaths.push_back("./archivos/exp1.txt");
    filePaths.push_back("./archivos/exp2.txt");
    filePaths.push_back("./archivos/exp3.txt");
    filePaths.push_back("./archivos/exp4.txt");
    filePaths.push_back("./archivos/exp5.txt");
    filePaths.push_back("./archivos/exp6.txt");
    filePaths.push_back("./archivos/exp7.txt");
    filePaths.push_back("./archivos/exp8.txt");


    for (int i = 1; i < max_threads + 1; i++) {
    // Cargar archivos con i thread
    hashMap = correrCargarArchivosConNThreads(i);

    // Calcular maximo con i thread
    correrMaximoParaleloConNThreads(i, hashMap);
    }
/* 
    // Cargar archivos con 1 thread
    hashMap = correrCargarArchivosConNThreads(1);

    // Calcular maximo con 1 thread
    correrMaximoParaleloConNThreads(1, hashMap);

    // Cargar archivos con 8 threads
    hashMap = correrCargarArchivosConNThreads(8);

    // Calcular maximo con 8 threads
    correrMaximoParaleloConNThreads(8, hashMap); */
}

int main(int argc, char **argv) {
    
    ofstream myfile;
    myfile.open ("./archivos/muestra.csv");
    myfile << "accion,threads,tiempo\n";
    myfile.close();
    cout << "Ejecutando experimento 1" << endl;
    experimento1();
    return 0;
}
