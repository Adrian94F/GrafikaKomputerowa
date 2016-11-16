/*************************************************************************************/
// Adrian Frydma�ski
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
// Sta�e i zmienne globalne:
const float pi = 3.14159265;							// pi
point3 **pTab;											// tablica na punkty
point3 **nVecTab;											// tablica na kolory
int n = 15;												// do poszia�u kwadratu jednostkowego
int model = 1;											// 1-punkty, 2-siatka, 3-kolorowe trojkaty
float squareLen = 1.0;									// d�ugo�� boku kwadratu jednostkowego
point3 v = { 0.05, 0.05, 0.05 };						// szybko�� obracania si�
static GLfloat theta[] = { 0.0, 0.0, 0.0 };

/*************************************************************************************/
// Funkcje wyliczaj�ce wsp�rz�dne punktu (u,v) w przestrzeni 3D
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
	return y - 5;										// obni�enie jajka, �eby si� �adnie mie�ci�o
}

float transformTo3D_z(float u, float v)
{
	float z, a = v * pi;
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a);
	return z;
}

float x_u(float u, float v)
{
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * cos(pi * v);
}

float x_v(float u, float v)
{
	return pi * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * sin(pi * v);
}

float y_u(float u, float v)
{
	return 640 * pow(u, 3) - 960 * pow(u, 2) + 320 * u;
}

float y_v(float u, float v)
{
	return 0;
}

float z_u(float u, float v)
{
	return (-450 * pow(u, 4) + 900 * pow(u, 3) - 810 * pow(u, 2) + 360 * u - 45) * sin(pi * v);
}

float z_v(float u, float v)
{
	return -pi * (90 * pow(u, 5) - 225 * pow(u, 4) + 270 * pow(u, 3) - 180 * pow(u, 2) + 45 * u) * cos(pi * v);
}

float normalVectorOfEgg_x(float u, float v)
{
	return y_u(u, v) * z_v(u, v) - z_u(u, v) * y_v(u, v);
}

float normalVectorOfEgg_y(float u, float v)
{
	return z_u(u, v) * x_v(u, v) - x_u(u, v) * z_v(u, v);
}

float normalVectorOfEgg_z(float u, float v)
{
	return x_u(u, v) * y_v(u, v) - y_u(u, v) * x_v(u, v);
}

/*************************************************************************************/
// Generowanie siatki punkt�w:
void eggGenerate()
{
	float step = squareLen / n;
	// wsp�rz�dne 2D -> 3D
	float u, v, length;
	for (int i = 0; i<n + 1; i++)
		for (int j = 0; j<n + 1; j++)
		{
			u = j * step;
			v = i * step;
			pTab[i][j][0] = transformTo3D_x(u, v);
			pTab[i][j][1] = transformTo3D_y(u, v);
			pTab[i][j][2] = transformTo3D_z(u, v);
			nVecTab[i][j][0] = normalVectorOfEgg_x(u, v);
			nVecTab[i][j][1] = normalVectorOfEgg_y(u, v);
			nVecTab[i][j][2] = normalVectorOfEgg_z(u, v);
			length = sqrt(nVecTab[i][j][0] * nVecTab[i][j][0] + nVecTab[i][j][1] * nVecTab[i][j][1] + nVecTab[i][j][2] * nVecTab[i][j][2]);

			for (int k = 0; k < 3; k++)
			{
				if (j < n / 2)
					nVecTab[i][j][k] *= -1;
				nVecTab[i][j][k] /= length;
			}
			if (length == 0)
				if (i ==0 || i == n+1)
				{
					nVecTab[i][j][0] = 0;
					nVecTab[i][j][1] = -1;
					nVecTab[i][j][2] = 0;
				}
				else if (i == n / 2)
				{
					nVecTab[i][j][0] = 0;
					nVecTab[i][j][1] = 1;
					nVecTab[i][j][2] = 0;
				}
		}
}

/*************************************************************************************/
// Renderowanie jajka
void Egg()
{
	// Generowanie siatki
	eggGenerate();

	// Ustawienie koloru bia�ego
	glColor3f(1.0, 1.0, 1.0);

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			// w jedn� stron�
			glBegin(GL_TRIANGLES);
			glNormal3fv(nVecTab[i][j + 1]);
			glVertex3fv(pTab[i][j + 1]);
			glNormal3fv(nVecTab[i + 1][j]);
			glVertex3fv(pTab[i + 1][j]);
			glNormal3fv(nVecTab[i + 1][j + 1]);
			glVertex3fv(pTab[i + 1][j + 1]);
			// w drug� stron�
			glNormal3fv(nVecTab[i][j]);
			glVertex3fv(pTab[i][j]);
			glNormal3fv(nVecTab[i + 1][j]);
			glVertex3fv(pTab[i + 1][j]);
			glNormal3fv(nVecTab[i][j + 1]);
			glVertex3fv(pTab[i][j + 1]);
			glEnd();
		}
}

void allocate()
{
	//Dynamiczna alokacja tablicy punktow
	pTab = new point3*[n + 1];
	for (int i = 0; i < n + 1; i++)
		pTab[i] = new point3[n + 1];

	//Dynamiczna alokacja tablicy i wygenerowanie kolorow losowych dla punktow
	nVecTab = new point3*[n + 1];
	for (int i = 0; i < n + 1; i++)
		nVecTab[i] = new point3[n + 1];
}

void relase()
{
	//Zwolnienie pami�ci
	for (int i = 0; i < n + 1; i++)
	{
		delete[] pTab[i];
		delete[] nVecTab[i];
		pTab[i] = 0;
		nVecTab[i] = 0;
	}
	delete[] pTab;
	delete[] nVecTab;
	pTab = 0;
	nVecTab = 0;
}

/*************************************************************************************/
// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)
void RenderScene()
{
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Czyszczenie macierzy bie��cej
	glLoadIdentity();

	//Rotacje
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);


	//Renderowanie jajka
	Egg();
	//glutSolidTeapot(2.5);

	//czajnik
	//glutSolidTeapot(2);

	// Przekazanie polece� rysuj�cych do wykonania
	glFlush();

	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja zwrotna dla klawiszy
void Keys(unsigned char key, int x, int y)
{
	// zmiana "rozdzielczo�ci" jajka
	if (key == '+' || key == '-' || key == 72 || key == 80)
	{
		relase();
		if (key == '+' || key == 72) n += 5;
		if ((key == '-' || key == 80) && n > 0) n -= 5;
		allocate();
	}
	// zmiana r�dko�ci kr�cenia si�
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

	glutPostRedisplay();								//od�wie�enie zawarto�ci okna
}

/*************************************************************************************/
// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);					// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

	/*************************************************************************************/
	// Definicja materia�u z jakiego zrobiony jest czajnik 
	GLfloat mat_ambient[] = { 1.0, 1.0, 0.0, 1.0 };			// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };			// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 0.0, 1.0 };		// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego                
	GLfloat mat_shininess = { 20.0 };						// wsp�czynnik n opisuj�cy po�ysk powierzchni

	/*************************************************************************************/
	// Definicja �r�d�a �wiat�a
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };		// po�o�enie �r�d�a
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia
															// Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
															// odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };		// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego
															// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant = { 1.0 };							// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji 
															// odleg�o�ci od �r�d�a
	GLfloat att_linear = { 0.05f };							// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji 
															// odleg�o�ci od �r�d�a
	GLfloat att_quadratic = { 0.001f };						// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji
															// odleg�o�ci od �r�d�a

	/*************************************************************************************/
	// Ustawienie patrametr�w materia�u
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametr�w �r�d�a
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
	// Ustawienie opcji systemu o�wietlania sceny 
	glShadeModel(GL_SMOOTH); // w�aczenie �agodnego cieniowania
	glEnable(GL_LIGHTING);   // w�aczenie systemu o�wietlenia sceny 
	glEnable(GL_LIGHT0);     // w��czenie �r�d�a o numerze 0
	glEnable(GL_DEPTH_TEST); // w��czenie mechanizmu z-bufora 
}

/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych obiekt�w w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal s� przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	// Deklaracja zmiennej AspectRatio  okre�laj�cej proporcj� wymiar�w okna
	GLfloat AspectRatio;

	if (vertical == 0)									// Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	// Ustawienie wielko�ciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical) 
	glViewport(0, 0, horizontal, vertical);

	// Prze��czenie macierzy bie��cej na macierz projekcji
	glMatrixMode(GL_PROJECTION);

	// Czyszcznie macierzy bie��cej    
	glLoadIdentity();

	// Wyznaczenie wsp�czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okre�lenie tak zwanej
	// przestrzeni ograniczaj�cej pozwalaj�cej zachowa� w�a�ciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj�cej s�u�y funkcja
	// glOrtho(...)           
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;

	if (horizontal <= vertical)
		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else
		glOrtho(-7.5*AspectRatio, 7.5*AspectRatio, -7.5, 7.5, 10.0, -10.0);
	// Prze��czenie macierzy bie��cej na macierz widoku modelu  
	glMatrixMode(GL_MODELVIEW);
	// Czyszcenie macierzy bie��cej                     
	glLoadIdentity();
}

/*************************************************************************************/
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
void main(void)
{
	//w��czenie polskich znak�w
	setlocale(LC_ALL, "");

	// zaalokuj tablic�
	allocate();

	// inicjowanie randa
	srand((unsigned)time(NULL));

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(800, 800);

	glutCreateWindow("O�wietlenie obiektow 3D");

	// Okre�lenie funkcji RenderScene jako funkcji zwrotnej (callback function)
	// B�dzie ona wywo�ywana za ka�dym razem, gdy zajdzie potrzba przeryswania okna
	glutDisplayFunc(RenderScene);

	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);

	// Funkcja MyInit() wykonuje wszelkie inicjalizacje konieczne przed przyst�pieniem do renderowania 
	MyInit();

	// W��czenie mechanizmu usuwania powierzchni niewidocznych
	glEnable(GL_DEPTH_TEST);

	// Funkcja zwrotna dla klawiatury
	glutKeyboardFunc(Keys);

	// Funkcja zwrotna obrotu
	glutIdleFunc(spinEgg);

	// g��wna petla GLUTa
	glutMainLoop();

	// zwolnij pami��
	relase();
}