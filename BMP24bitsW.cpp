#include <process.h>
#include <Windows.h>
#include <stdio.h>       
#include "BMP.h"

//Solo modifica el numero de hilos
#define NUM_THREADS 8


unsigned __stdcall hilograyscale(void* param);

double timeval_diff(LARGE_INTEGER* a, LARGE_INTEGER* b) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return (double)(a->QuadPart - b->QuadPart) / freq.QuadPart;
}

int main(int argc, char** argv) {
    LARGE_INTEGER t_ini, t_fin;
    double secs;
    BMP bmp("nature.bmp");

    QueryPerformanceCounter(&t_ini);

    HANDLE threads[NUM_THREADS]; // Array para almacenar los identificadores de los hilos

    // Creamos 8 hilos adicionales para procesamiento en paralelo
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = (HANDLE)_beginthreadex(NULL, 0, hilograyscale, &bmp, 0, NULL);
    }

    // Esperamos a que todos los hilos terminen
    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);

    for (int i = 0; i < NUM_THREADS; ++i) {
        CloseHandle(threads[i]);
    }

    QueryPerformanceCounter(&t_fin);

    secs = timeval_diff(&t_fin, &t_ini);
    printf("%.10g\n", secs * 1000.0);

    bmp.save("grises.bmp");

    return 0;
}

unsigned __stdcall hilograyscale(void* param) {
	BMP* bmp = (BMP*)param;
	bmp->fromRGBtoGRAY();
	return 0;
}