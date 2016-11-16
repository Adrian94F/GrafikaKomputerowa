/*************************************************************************************/

// Troch� bardziej skomplikowany program wykorzystuj�cy funkcje biblioteki OpenGL

/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <iostream>
#include <time.h>
using namespace std;
/*************************************************************************************/

//globalna glebokosc dywanu
int glebokosc, przemieszczenie;

//losowy float od 0 do 1
float rnd01()
{
	return ((float)rand() / (RAND_MAX));
}

/*************************************************************************************/

//Funkcja dywanowa, rekurencyjna
void dywanWaclawa(float _x0, float _y0, float _x1, float _y1, int glebokosc, int przemieszczenie)
{
	if (glebokosc > 0)
	{
		//tablica punkt�w dla mniejszych dywan�w Wac�awa
		float x[4], y[4];

		//definiowanie wierzcho�k�w mniejszych dywanik�w
		x[0] = _x0;
		x[1] = (_x1 - _x0) / 3.0f;
		x[2] = 2.0f * x[1];
		x[3] = 3.0f * x[1];
		for (int i = 1; i < 4; i++)
			x[i] += x[0];

		y[0] = _y0;
		y[1] = (_y1 - _y0) / 3.0f;
		y[2] = 2.0f * y[1];
		y[3] = 3.0f * y[1];
		for (int i = 1; i < 4; i++)
			y[i] += y[0];

		//deformacja wsp�rz�dnych
		//int max[2] = { (_x1 - _x0) * przemieszczenie / 100, (_y1 - _y0) * przemieszczenie / 100 };
		//for (int i = 0; i < 4; i++)
		//{
		//	x[i] += max[0] * (rnd01() - 0.5);
		//	y[i] += max[1] * (rnd01() - 0.5);
		//}
		
		//zmniejszenie g��boko�ci dla rekurencji, kt�ra ma si� w ko�cu sko�czy�
		glebokosc--;

		//rysowanie mniejszych dywanik�w
		//p�tla po wysoko�ci
		for (int i = 0; i < 3; i++)
		{
			//p�tla po szeroko�ci
			for (int j = 0; j < 3; j++)
			{
				if (i != 1 || j != 1)
					dywanWaclawa(x[i], y[j], x[i+1], y[j+1], glebokosc, przemieszczenie);
			}
		}

	}
	else
	{
		//ustawienie aktualnego koloru rysowania
		glColor3f(rnd01(), rnd01(), rnd01());

		//rysowanie kwadratu
		glRectf(_x0, _y0, _x1, _y1);
	}
}


/*************************************************************************************/

// Funkcaja okre�laj�ca, co ma by� rysowane 
// (zawsze wywo�ywana, gdy trzeba przerysowa� scen�)


void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym

	dywanWaclawa(-90.0f, 90.0f, 90.0f, -90.0f, glebokosc, przemieszczenie);

	glFlush();
	// Przekazanie polece� rysuj�cych do wykonania
}
/*************************************************************************************/

// Funkcja ustalaj�ca stan renderowania


void MyInit(void)
{
	glClearColor(0, 0, 0, 1.0f);
	// Kolor okna wn�trza okna - ustawiono na szary

	//dla randa
	srand(time(NULL));
}
/*************************************************************************************/

// Funkcja s�u��ca do kontroli zachowania proporcji rysowanych obiekt�w
// niezale�nie od rozmiar�w okna graficznego


void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szeroko�� i wysoko�� okna) s�
// przekazywane do funkcji za ka�dym razem, gdy zmieni si� rozmiar okna
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio okre�laj�cej proporcj� wymiar�w okna

	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielko�ciokna okna urz�dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Okre�lenie uk�adu wsp�rz�dnych obserwatora
	glLoadIdentity();
	// Okre�lenie przestrzeni ograniczaj�cej
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wsp�czynnika proporcji okna
	// Gdy okno na ekranie nie jest kwadratem wymagane jest 
	// okre�lenie okna obserwatora. 
	// Pozwala to zachowa� w�a�ciwe proporcje rysowanego obiektu
	// Do okre�lenia okna obserwatora s�u�y funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okre�lenie uk�adu wsp�rz�dnych     
	glLoadIdentity();
}
/*************************************************************************************/

// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli


void main(void)
{
	cout << "Podaj glebokosc\n";
	cin >> glebokosc;
	/*cout << "Podaj przemieszczenie w %\n";
	cin >> przemieszczenie;
	if (przemieszczenie < 0)
		przemieszczenie = -przemieszczenie;*/

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wy�wietlania
	// GLUT_SINGLE - pojedynczy bufor wy�wietlania
	// GLUT_RGBA - model kolor�w RGB

	glutCreateWindow("Lab2 - 209865");
	// Utworzenie okna i okre�lenie tre�ci napisu w nag��wku okna

	glutDisplayFunc(RenderScene);
	// Okre�lenie, �e funkcja RenderScene b�dzie funkcj� zwrotn� (callback)
	// Biblioteka GLUT b�dzie wywo�ywa�a t� funkcj� za ka�dym razem, gdy
	// trzeba b�dzie przerysowa� okno


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy�ej) wykonuje wszelkie  
	// inicjalizacje konieczneprzed przyst�pieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
/*************************************************************************************/