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
struct timespec start_experimento, stop_experimento;
vector<string> filePaths = {};
int max_threads = 8;

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
    /* std::cout << "Tiempo elapsado con " << threads << " threads ejecutando " << origen << std::endl;
    printf("%ld.%09ld\n", (long)(stop.tv_sec - start.tv_sec),
        stop.tv_nsec - start.tv_nsec); */
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
    //std::cout << maximo.first << " " << maximo.second << std::endl;
    calcularTiempo(threads, start, stop, "Máximo");
}

void experimento1(){
    // Experimento 1
    // Con el doble de threads deberia tardar la mitad de tiempo    
    HashMapConcurrente hashMap;


    for (int i = 1; i < max_threads + 1; i++) {
        // Cargar archivos con i thread
        hashMap = correrCargarArchivosConNThreads(i);

        // Calcular maximo con i thread
        correrMaximoParaleloConNThreads(i, hashMap);
    }

}

void CreamosVectorFiles() {
    filePaths.push_back("./archivos/exp1.txt");
    filePaths.push_back("./archivos/exp2.txt");
    filePaths.push_back("./archivos/exp3.txt");
    filePaths.push_back("./archivos/exp4.txt");
    filePaths.push_back("./archivos/exp5.txt");
    filePaths.push_back("./archivos/exp6.txt");
    filePaths.push_back("./archivos/exp7.txt");
    filePaths.push_back("./archivos/exp8.txt");
}

int main(int argc, char **argv) {
    
    CreamosVectorFiles();

    ofstream myfile;
    myfile.open ("./archivos/muestra.csv");
    myfile << "accion,threads,tiempo\n";
    myfile.close();
    cout << "Ejecutando experimentos" << endl;
    for (int i = 0; i < 1000; i++) {
        
        if( clock_gettime( CLOCK_REALTIME, &start_experimento) == -1 ) {
          perror( "clock gettime" );
          exit( EXIT_FAILURE );
        }
        cout << "Ejecutando experimento " << i + 1 << endl;
        experimento1();
        if( clock_gettime( CLOCK_REALTIME, &stop_experimento) == -1 ) {
          perror( "clock gettime" );
          exit( EXIT_FAILURE );
        }
        printf("Tiempo de ejecucion %d: %ld.%09ld\n", i + 1, (long)(stop_experimento.tv_sec - start_experimento.tv_sec),
        (stop_experimento.tv_nsec - start_experimento.tv_nsec) < 0 ?
        999999999 + (stop_experimento.tv_nsec - start_experimento.tv_nsec) : (stop_experimento.tv_nsec - start_experimento.tv_nsec));
    }
    return 0;
}
