#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <fcntl.h> 
#include <stdio.h>
#include <utility>
#include <time.h>
#include <math.h>

using namespace std;

#define nOfChildren 2
#define clear() printf("\033[H\033[J")

typedef float point[3];										// wektor 3 liczb

typedef struct
{
	int x_size;
	int y_size;
	sem_t waitForData; 										// semafor dla dzieci, by czekały na dane
	sem_t waitForRender;									// semafor dla rodzica, by czekał na obliczenia gówniarzy
}shmstruct;