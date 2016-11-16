/*************************************************************************************/
// Adrian Frydmañski
// 209865
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <time.h>
#include <iostream>
using namespace std;

typedef float point3[3];

/*************************************************************************************/
// Sta³e i zmienne globalne:
const float pi = 3.14159265;							// pi
point3 **pTab;											// tablica na punkty
point3 **pRGB;											// tablica na kolory
int n = 15;												// do poszia³u kwadratu jednostkowego
int model = 1;											// 1-punkty, 2-siatka, 3-kolorowe trojkaty
float squareLen = 1.0;									// d³ugoœæ boku kwadratu jednostkowego
float len = 2;											// d³ugoœæ osi
point3 v = {0.05, 0.05, 0.05};							// szybkoœæ obracania siê
static GLfloat theta[] = { 0.0, 0.0, 0.0 };

/*************************************************************************************/
// Funkcje wyliczaj¹ce wspó³rzêdne punktu (u,v) w przestrzeni 3D
float transformTo3D_x(float u, float v) 
{
	float x, a = v * pi;
	x = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(a);
	return x;
}

float transformTo3D_y(float u, float v) 
{
	float y;
	y = 160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2);
	return y - 5;										// obni¿enie jajka, ¿eby siê ³adnie mieœci³o
}

float transformTo3D_z(float u, float v) 
{
	float z, a = v * pi;
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a);
	return z;
}

/*************************************************************************************/
// Generowanie siatki punktów:
void eggGenerate()
{
	float step = squareLen / n;
	// wspó³rzêdne 2D -> 3D
	float u, v;
	for (int i = 0; i<n + 1; i++) 
		for (int j = 0; j<n + 1; j++)
		{
			u = j * step;
			v = i * step;
			pTab[i][j][0] = transformTo3D_x(u, v);
			pTab[i][j][1] = transformTo3D_y(u, v);
			pTab[i][j][2] = transformTo3D_z(u, v);
		}
}

/*************************************************************************************/
// Renderowanie jajka
void Egg() 
{
	// Generowanie siatki
	eggGenerate();

	// Ustawienie koloru bia³ego
	glColor3f(1.0, 1.0, 1.0);

	// switch w zale¿noœci od modelu
	switch (model) 
	{
	case 1:												// punkty
	{
		glBegin(GL_POINTS);
			for (int i = 0; i<n; i++) 
				for (int j = 0; j<n; j++) 
					glVertex3fv(pTab[i][j]);
		glEnd();
	} 
	break;
	case 2:												// siatka
	{
		for (int i = 0; i < n; i++) 
			for (int j = 0; j < n; j++) 
			{
				glBegin(GL_LINES);
					// pionowo
					glVertex3fv(pTab[i][j]);
					glVertex3fv(pTab[i][j + 1]);
					// poziomo
					glVertex3fv(pTab[i][j]);
					glVertex3fv(pTab[i + 1][j]);
					// w prawo w dó³
					glVertex3fv(pTab[i][j]);
					glVertex3fv(pTab[i + 1][j + 1]);
				glEnd();
			}
	} 
	break;
	case 3:												// trójk¹ty
	{
		for (int i = 0; i < n; i++) 
			for (int j = 0; j < n; j++) 
			{
				// w jedn¹ stronê
				glBegin(GL_TRIANGLES);
				glColor3fv(pRGB[i][j + 1]);
				glVertex3fv(pTab[i][j + 1]);
				glColor3fv(pRGB[i + 1][j]);
				glVertex3fv(pTab[i + 1][j]);
				glColor3fv(pRGB[i + 1][j + 1]);
				glVertex3fv(pTab[i + 1][j + 1]);
				// w drug¹ stronê
				glColor3fv(pRGB[i][j]);
				glVertex3fv(pTab[i][j]);
				glColor3fv(pRGB[i + 1][j]);
				glVertex3fv(pTab[i + 1][j]);
				glColor3fv(pRGB[i][j + 1]);
				glVertex3fv(pTab[i][j + 1]);
				glEnd();
			}
	}
	break;
	}
}

/*************************************************************************************/
// Funkcja rysuj¹ca osie uk³adu wspó³rzêdnych
void Axes(float len)
{
	// pocz¹tek i koniec obrazu osi x
	point3  x_min = { -len, 0.0, 0.0 };
	point3  x_max = { len, 0.0, 0.0 };

	// pocz¹tek i koniec obrazu osi y
	point3  y_min = { 0.0, -len, 0.0 };
	point3  y_max = { 0.0,  len, 0.0 };

	// pocz¹tek i koniec obrazu osi z
	point3  z_min = { 0.0, 0.0, -len };
	point3  z_max = { 0.0, 0.0,  len };

	glColor3f(1.0f, 0.0f, 0.0f);						// kolor rysowania osi - czerwony
	glBegin(GL_LINES);									// rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);						// kolor rysowania - zielony
	glBegin(GL_LINES);									// rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);						// kolor rysowania - niebieski
	glBegin(GL_LINES);									// rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

void allocate()
{
	//Dynamiczna alokacja tablicy punktow
	pTab = new point3*[n + 1];
	for (int i = 0; i < n + 1; i++)
		pTab[i] = new point3[n + 1];

	//Dynamiczna alokacja tablicy i wygenerowanie kolorow losowych dla punktow
	pRGB = new point3*[n + 1];
	for (int i = 0; i < n + 1; i++)
		pRGB[i] = new point3[n + 1];
	for (int i = 0; i < n + 1; i++)
		for (int j = 0; j < n + 1; j++)
		{
			pRGB[i][j][0] = ((float)(rand() % 10) + 1) / 10;
			pRGB[i][j][1] = ((float)(rand() % 10) + 1) / 10;
			pRGB[i][j][2] = ((float)(rand() % 10) + 1) / 10;
		}
}

void relase()
{
	//Zwolnienie pamiêci
	for (int i = 0; i < n + 1; i++)
	{
		delete[] pTab[i];
		delete[] pRGB[i];
		pTab[i] = 0;
		pRGB[i] = 0;
	}
	delete[] pTab;
	delete[] pRGB;
	pTab = 0;
	pRGB = 0;
}

/*************************************************************************************/
// Funkcja okreœlaj¹ca co ma byæ rysowane (zawsze wywo³ywana, gdy trzeba przerysowaæ scenê)
void RenderScene()
{
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Czyszczenie macierzy bie¿¹cej
	glLoadIdentity();

	//Rotacje
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);

	//Renderowanie osi
	Axes(len);

	//Renderowanie jajka
	Egg();

	// Przekazanie poleceñ rysuj¹cych do wykonania
	glFlush();

	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja zwrotna dla klawiszy
void Keys(unsigned char key, int x, int y)
{
	// zmiana modelu wyœwietlania
	if (key == 'p' || key == '1') model = 1;
	if (key == 's' || key == '2') model = 2;
	if (key == 't' || key == '3') model = 3;
	// zmiana "rozdzielczoœci" jajka
	if ( key == '+' || key == '-' || key == 72 || key == 80)
	{
		relase();
		if (key == '+' || key == 72) n += 5;
		if ((key == '-' || key == 80) && n > 0) n -= 5;
		allocate();
	}
	// zmiana d³ugoœci osi wspó³rzêdnych
	if (key == '*') len += 0.25;
	if (key == '/' && len > 0) len -= 0.25;
	// zmiana rêdkoœci krêcenia siê
	if (key == '7') v[0] += 0.05;
	if (key == '4') v[0] -= 0.05;
	if (key == '8') v[1] += 0.05;
	if (key == '5') v[1] -= 0.05;
	if (key == '9') v[2] += 0.05;
	if (key == '6') v[2] -= 0.05;
	RenderScene();
}

/*************************************************************************************/
// Funkcja zwrotna dla obrotu
void spinEgg()
{
	theta[0] -= v[0];
	if (theta[0] > 360.0) theta[0] -= 360.0;
	theta[1] -= v[1];
	if (theta[1] > 360.0) theta[1] -= 360.0;
	theta[2] -= v[2];
	if (theta[2] > 360.0) theta[2] -= 360.0;

	glutPostRedisplay();								//odœwie¿enie zawartoœci okna
}

/*************************************************************************************/
// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny
}

/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie sta³ych proporcji rysowanych obiektów w przypadku zmiany rozmiarów okna.
// Parametry vertical i horizontal s¹ przekazywane do funkcji za ka¿dym razem gdy zmieni siê rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	// Deklaracja zmiennej AspectRatio  okreœlaj¹cej proporcjê wymiarów okna
	GLfloat AspectRatio;

	if (vertical == 0)									// Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	// Ustawienie wielkoœciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical) 
	glViewport(0, 0, horizontal, vertical);

	// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji
	glMatrixMode(GL_PROJECTION);

	// Czyszcznie macierzy bie¿¹cej    
	glLoadIdentity();         

	// Wyznaczenie wspó³czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okreœlenie tak zwanej
	// przestrzeni ograniczaj¹cej pozwalaj¹cej zachowaæ w³aœciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj¹cej s³u¿y funkcja
	// glOrtho(...)           
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;

	if (horizontal <= vertical)
		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else
		glOrtho(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5, 10.0, -10.0);
	// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu  
	glMatrixMode(GL_MODELVIEW);
	// Czyszcenie macierzy bie¿¹cej                     
	glLoadIdentity();
}

/*************************************************************************************/
// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
void main(void)
{
	//w³¹czenie polskich znaków
	setlocale(LC_ALL, "");

	cout << "Sterowanie (najwygodniej na klawiaturze numerycznej):\n"
		" p/1     punkty\n"
		" s/2     siatka\n"
		" t/3     trójk¹ty\n"
		" +       zwiêksz rozdzielczosc jajka\n"
		" -       zmniejsz rozdzielczoœæ jajka\n"
		" *       wyd³u¿ osie\n"
		" /       skróæ osie\n"
		" 7/8/9   zwiêksz prêdkoœæ obrotu w osi X/Y/Z\n"
		" 4/5/6   zmniejsz prêdkoœæ obrotu w osi X/Y/Z\n";

	// zaalokuj tablicê
	allocate();

	// inicjowanie randa
	srand((unsigned)time(NULL));

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1024, 800);

	glutCreateWindow("Modelowanie obiektow 3D - jajko (instrukcja w konsoli)");

	// Okreœlenie funkcji RenderScene jako funkcji zwrotnej (callback function)
	// Bêdzie ona wywo³ywana za ka¿dym razem, gdy zajdzie potrzba przeryswania okna
	glutDisplayFunc(RenderScene);

	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za zmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);

	// Funkcja MyInit() wykonuje wszelkie inicjalizacje konieczne przed przyst¹pieniem do renderowania 
	MyInit();

	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych
	glEnable(GL_DEPTH_TEST);

	// Funkcja zwrotna dla klawiatury
	glutKeyboardFunc(Keys);

	// Funkcja zwrotna obrotu
	glutIdleFunc(spinEgg);

	// g³ówna petla GLUTa
	glutMainLoop();

	// zwolnij pamiêæ
	relase();
}