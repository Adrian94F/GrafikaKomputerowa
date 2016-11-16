#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>

/*************************************************************************************/
// zmienne globalne
typedef float point3[3];
static GLfloat viewer[] = { 0.0, 0.0, 10.0 };			// inicjalizacja po�o�enia obserwatora
static GLfloat focus[] = { 0.0, 0.0, 0.0 };				// inicjalizacja punktu, na kt�ry patrzy obserwator
static GLfloat cam[] = { 0.0, 1.0, 0.0 };				// inicjalizacja pozycji kamery

const float pi = 3.14159265;							// pi
point3 **pTab;											// tablica na punkty
point3 **pRGB;											// tablica na kolory
int n = 50;												// do poszia�u kwadratu jednostkowego
int model = 3;											// 1-punkty, 2-siatka, 3-kolorowe trojkaty
float squareLen = 1.0;									// d�ugo�� boku kwadratu jednostkowego
float len = 2;											// d�ugo�� osi
point3 v = { 0.05, 0.05, 0.05 };						// szybko�� obracania si�

static GLfloat theta_x = 0.0;							// k�t obrotu obiektu
static GLfloat theta_y = 0.0;							// k�t obrotu obiektu
static GLfloat pix2angle_x;								// przelicznik pikseli na stopnie
static GLfloat pix2angle_y;								// przelicznik pikseli na stopnie

static GLint status = 0;								// stan klawiszy myszy (o - brak, 1 - lewy)

static int x_pos_old = 0;								// poprzednia pozycja kursora myszy
static int y_pos_old = 0;								// poprzednia pozycja kursora myszy

static int delta_x = 0;									// r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy 
static int delta_y = 0;									// r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy 

/*************************************************************************************/
// Funkcja rysuj�ca osie uk�adu wsp�rz�dnych
void Axes(float len)
{
	// pocz�tek i koniec obrazu osi x
	point3  x_min = { -len, 0.0, 0.0 };
	point3  x_max = { len, 0.0, 0.0 };

	// pocz�tek i koniec obrazu osi y
	point3  y_min = { 0.0, -len, 0.0 };
	point3  y_max = { 0.0,  len, 0.0 };

	// pocz�tek i koniec obrazu osi z
	point3  z_min = { 0.0, 0.0, -len };
	point3  z_max = { 0.0, 0.0,  len };

	glColor3f(1.0f, 0.0f, 0.0f);													// kolor rysowania osi - czerwony
	glBegin(GL_LINES);																// rysowanie osi x
	glVertex3fv(x_min);
	glVertex3fv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);													// kolor rysowania - zielony
	glBegin(GL_LINES);																// rysowanie osi y
	glVertex3fv(y_min);
	glVertex3fv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);													// kolor rysowania - niebieski
	glBegin(GL_LINES);																// rysowanie osi z
	glVertex3fv(z_min);
	glVertex3fv(z_max);
	glEnd();
}

/*************************************************************************************/
// Funkcje wyliczaj?ce wsp�?rz?dne punktu (u,v) w przestrzeni 3D
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
	return y - 5;										// obni�enie jajka, �eby si? �adnie mie�ci�o
}

float transformTo3D_z(float u, float v)
{
	float z, a = v * pi;
	z = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(a);
	return z;
}

/*************************************************************************************/
// Generowanie siatki punkt�w:
void eggGenerate()
{
	float step = squareLen / n;
	// wsp�rz?dne 2D -> 3D
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

	// Ustawienie koloru bia�ego
	glColor3f(1.0, 1.0, 1.0);

	// switch w zale?no�ci od modelu
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
				// w prawo w d�
				glVertex3fv(pTab[i][j]);
				glVertex3fv(pTab[i + 1][j + 1]);
				glEnd();
			}
	}
	break;
	case 3:												// tr�jk?ty
	{
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
			{
				// w jedn? stron?
				glBegin(GL_TRIANGLES);
				glColor3fv(pRGB[i][j + 1]);
				glVertex3fv(pTab[i][j + 1]);
				glColor3fv(pRGB[i + 1][j]);
				glVertex3fv(pTab[i + 1][j]);
				glColor3fv(pRGB[i + 1][j + 1]);
				glVertex3fv(pTab[i + 1][j + 1]);
				// w drug? stron?
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
	//Zwolnienie pami?ci
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
// Funkcja okre�laj�ca co ma by� rysowane (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);														// Czyszczenie okna aktualnym kolorem czyszcz�cym

	glLoadIdentity();																						// Czyszczenie macierzy bie��cej

	gluLookAt(viewer[0], viewer[1], viewer[2], focus[0], focus[1], focus[2], cam[0], cam[1], cam[3]);		// Zdefiniowanie po�o�enia obserwatora

	if (status == 1)																						// je�li lewy klawisz myszy wci�ni�ty
	{
		theta_x += delta_x*pix2angle_x;																		// modyfikacja k�ta obrotu o kat proporcjonalny
		theta_y += delta_y*pix2angle_y;																		// do r�nicy po�o�e� kursora myszy
	}
	else if (status == 2)																					// je�li prawy klawisz myszy wci�ni�ty
	{	
		viewer[2] += delta_y;																				// modyfikacja polozenia Z obserwatora (zoom)
		if (viewer[2] <= 7)																					// ograniczenie zblizenia
			viewer[2] = 7;
		if (viewer[2] >= 200)																				// ograniczenie oddalenia
			viewer[2] = 200;
	}

	glRotatef(theta_x, 0.0, 1.0, 0.0);																		//obr�t obiektu o nowy k�t
	glRotatef(theta_y, 1.0, 0.0, 0.0);																		//obr�t obiektu o nowy k�t

	Axes(2);																								// Narysowanie osi przy pomocy funkcji zdefiniowanej powy�ej

	Egg();																									// Narysowanie jajka

	glFlush();																								// Przekazanie polece� rysuj�cych do wykonania

	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja zwrotna dla klawiszy
void Keys(unsigned char key, int x, int y)
{
	// zmiana modelu wy?wietlania
	if (key == 'p' || key == '1') model = 1;
	if (key == 's' || key == '2') model = 2;
	if (key == 't' || key == '3') model = 3;
	// zmiana "rozdzielczo?ci" jajka
	if (key == '+' || key == '-' || key == 72 || key == 80)
	{
		relase();
		if (key == '+' || key == 72) n += 5;
		if ((key == '-' || key == 80) && n > 0) n -= 5;
		allocate();
	}
	RenderScene();
}

/*************************************************************************************/
// Funkcja ustalaj�ca stan renderowania
void MyInit(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny
}

/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie sta�ych proporcji rysowanych obiekt�w w przypadku zmiany rozmiar�w okna.
// Parametry vertical i horizontal s� przekazywane do funkcji za ka�dym razem gdy zmieni si� rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angle_x = 360.0 / (float)horizontal;  // przeliczenie pikseli na stopnie
	pix2angle_y = 360.0 / (float)vertical;  // przeliczenie pikseli na stopnie

	// Deklaracja zmiennej AspectRatio  okre�laj�cej proporcj� wymiar�w okna
	GLfloat AspectRatio;

	if (vertical == 0)												// Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;

	// Ustawienie wielko�ciokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical) 
	glViewport(0, 0, horizontal, vertical);

	// Prze��czenie macierzy bie��cej na macierz projekcji
	glMatrixMode(GL_PROJECTION);
   
	glLoadIdentity();												// Czyszcznie macierzy bie��cej 

	// Wyznaczenie wsp�czynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest okre�lenie tak zwanej
	// przestrzeni ograniczaj�cej pozwalaj�cej zachowa� w�a�ciwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczj�cej s�u�y funkcja
	// glOrtho(...)           
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;

	gluPerspective(80, AspectRatio, 1.0, 200.0);					// Ustawienie parametr�w dla rzutu perspektywicznego
																	// (fovy, aspect, zNear, zFar)
																	// (k�t, stosunek szeroko�ci do wysoko�ci, odleg�o�� od okna, odleg�o�� od rzutni)

	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci
	// relacji pomi�dzy wysoko�ci� i szeroko�ci� okna
	glViewport(0, 0, horizontal, vertical);

	// Prze��czenie macierzy bie��cej na macierz widoku modelu  
	glMatrixMode(GL_MODELVIEW);
	// Czyszcenie macierzy bie��cej                     
	glLoadIdentity();
}

/*************************************************************************************/
// Funkcja "bada" stan myszy i ustawia warto�ci odpowiednich zmiennych globalnych
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		x_pos_old = x;												// przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		y_pos_old = y;
		status = 1;													// wci�ni�ty zosta� lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		y_pos_old = y;												// przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		status = 2;													//wci�ni�ty zosta� prawy klawisz myszy
	}
	else
		status = 0;													// nie zosta� wci�ni�ty �aden klawisz 
}

/*************************************************************************************/
// Funkcja "monitoruje" po�o�enie kursora myszy i ustawia warto�ci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;										// obliczenie r�nicy po�o�enia kursora myszy
	x_pos_old = x;													// podstawienie bie��cego po�o�enia jako poprzednie
	delta_y = y - y_pos_old;										// obliczenie r�nicy po�o�enia kursora myszy
	y_pos_old = y;													// podstawienie bie��cego po�o�enia jako poprzednie
	glutPostRedisplay();											// przerysowanie obrazu sceny
}

/*************************************************************************************/
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
void main(void)
{
	// zaalokuj tablic?
	allocate();

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(1000, 600);

	glutCreateWindow("Rzutowanie perspektywiczne - 209865");

	glutMouseFunc(Mouse);											// Ustala funkcj� zwrotn� odpowiedzialn� za badanie stanu myszy
	
	glutMotionFunc(Motion);											// Ustala funkcj� zwrotn� odpowiedzialn� za badanie ruchu myszy

	glutDisplayFunc(RenderScene);									// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn�
																	// (callback function).  B�dzie ona wywo�ywana za ka�dym razem 
																	// gdy zajdzie potrzeba przerysowania okna

	glutReshapeFunc(ChangeSize);									// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna                       

	MyInit();														// Funkcja MyInit() (zdefiniowana powy�ej) wykonuje wszelkie
																	// inicjalizacje konieczne  przed przyst�pieniem do renderowania
	glEnable(GL_DEPTH_TEST);										// W��czenie mechanizmu usuwania niewidocznych element�w sceny

	glutKeyboardFunc(Keys);											// Funkcja zwrotna dla klawiatury

	glutMainLoop();													// Funkcja uruchamia szkielet biblioteki GLUT
																	// zwolnij pamiec
	relase();
}