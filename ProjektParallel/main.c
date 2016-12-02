#include "header.h"

long lastFrame = 0;						// Czas zakończenia poprzedniej ramki (ms)

int pid[nOfChildren];
key_t key;
int shmd;
int shmi;
int fifo;
char buffer[64] = {};
int size;
shmstruct *data;
point **image;
char myfifo[] = "fifo";	

static volatile int keepRunning = 1;

void intHandler(int dummy) 
{
	for (int i = 0; i < nOfChildren; i++)
		kill(pid[i], SIGKILL);
	shmdt(data);
	shmctl(shmd, IPC_RMID, NULL);
	shmdt(image);
	shmctl(shmi, IPC_RMID, NULL);
}

int init(int x, int y)
{
	signal(SIGINT, intHandler);			// Zarejestrowanie obsługi sygnału
	signal(SIGKILL, intHandler);

	key = ftok("semmem", 0);			// Pamięć współdzielona
	shmd = shmget(key, sizeof(shmstruct), 0777 | IPC_CREAT);
	data = (shmstruct*)shmat(shmd, 0, 0);
	if (data == NULL)
		perror("shmat");
	data->x_size = x;
	data->y_size = y;
	if(sem_init(&(data->waitForData), 1, nOfChildren))
		perror("mutex");
	if(sem_init(&(data->waitForRender), 1, 0))
		perror("mutex");

	key = ftok("imamem", 0);			// Pamięć współdzielona
	shmi = shmget(key, x*y*sizeof(point), 0777 | IPC_CREAT);
	image = (point**)shmat(shmd, 0, 0);
	if (image == NULL)
	perror("shmat");

	return 0;
}

void child(int x0, int x1, int y0, int y1, int id)							// Zapis do pamięci współdzielonej
{
	while(true)
	{
		cout << " Child " << id << " is waiting..." << endl;
		sem_wait(&(data->waitForData));
		cout << " Child " << id << " is rendering from (" << x0 << "," << y0 << ") to (" << x1 << "," << y1 << ")" << endl;
		for (int x = x0; x <= x1; x++)
		{
			cout << " *" << endl;
			for (int y = y0; y <= y1; y++)
			{
				cout << "  *" << endl;
				image[x][y][0] = id;
			}
		}
		cout << " Child " << id << " rendered image..." << endl;
		sem_post(&(data->waitForRender));
	}
}

int main()								// Funkcja główna
{
	int sizex = 20;
	int sizey = 20;
	if (init(sizex, sizey) == -1)				// Inicjalizacja
		return -1;
	int x0 = 0;
	int x1 = sizex / nOfChildren;
	int y0 = 0;
	int y1 = sizey;
	for (int i = 0; i < nOfChildren; i++)
	{
		pid[i] = fork();
		switch(pid[i])
		{
			case -1:					// Błąd
				return -1;
			case 0:						// Dziecko
				child(x0, x1, y0, y1, i);
				break;
		}
		x0 = x1;
		x1 += sizex / nOfChildren;
		if (i == nOfChildren - 1)
			x1 = sizex;

	}
	cout << "Parent" << endl;
	while(true)
	{
		cout << "Data ready" << endl;
		for (int i = 0; i < nOfChildren; i++)
			sem_post(&(data->waitForData));
		for (int i = 0; i < nOfChildren; i++)
			sem_wait(&(data->waitForRender));	// czekanie na render
		cout << "Got data, displaying" << endl;
		for (int y = y0; y <= y1; y++)
		{
			for (int x = x0; x <= x1; x++)
			{
				cout << image[x][y][0] << " ";
			}
			cout << endl;
		}
		sleep(10);
	}
	// usunięcie pamięci współdzielonej
	intHandler(0);

	return 0;
}
