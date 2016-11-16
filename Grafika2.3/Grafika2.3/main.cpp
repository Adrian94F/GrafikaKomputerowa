/*************************************************************************************/

// Trochê bardziej skomplikowany program wykorzystuj¹cy funkcje biblioteki OpenGL

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
		//tablica punktów dla mniejszych dywanów Wac³awa
		float x[4], y[4];

		//definiowanie wierzcho³ków mniejszych dywaników
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

		//deformacja wspó³rzêdnych
		//int max[2] = { (_x1 - _x0) * przemieszczenie / 100, (_y1 - _y0) * przemieszczenie / 100 };
		//for (int i = 0; i < 4; i++)
		//{
		//	x[i] += max[0] * (rnd01() - 0.5);
		//	y[i] += max[1] * (rnd01() - 0.5);
		//}
		
		//zmniejszenie g³êbokoœci dla rekurencji, która ma siê w koñcu skoñczyæ
		glebokosc--;

		//rysowanie mniejszych dywaników
		//pêtla po wysokoœci
		for (int i = 0; i < 3; i++)
		{
			//pêtla po szerokoœci
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

// Funkcaja okreœlaj¹ca, co ma byæ rysowane 
// (zawsze wywo³ywana, gdy trzeba przerysowaæ scenê)


void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym

	dywanWaclawa(-90.0f, 90.0f, 90.0f, -90.0f, glebokosc, przemieszczenie);

	glFlush();
	// Przekazanie poleceñ rysuj¹cych do wykonania
}
/*************************************************************************************/

// Funkcja ustalaj¹ca stan renderowania


void MyInit(void)
{
	glClearColor(0, 0, 0, 1.0f);
	// Kolor okna wnêtrza okna - ustawiono na szary

	//dla randa
	srand(time(NULL));
}
/*************************************************************************************/

// Funkcja s³u¿¹ca do kontroli zachowania proporcji rysowanych obiektów
// niezale¿nie od rozmiarów okna graficznego


void ChangeSize(GLsizei horizontal, GLsizei vertical)
// Parametry horizontal i vertical (szerokoœæ i wysokoœæ okna) s¹
// przekazywane do funkcji za ka¿dym razem, gdy zmieni siê rozmiar okna
{
	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio okreœlaj¹cej proporcjê wymiarów okna

	if (vertical == 0)
		// Zabezpieczenie pzred dzieleniem przez 0
		vertical = 1;

	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkoœciokna okna urz¹dzenia (Viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)

	glMatrixMode(GL_PROJECTION);
	// Okreœlenie uk³adu wspó³rzêdnych obserwatora
	glLoadIdentity();
	// Okreœlenie przestrzeni ograniczaj¹cej
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie wspó³czynnika proporcji okna
	// Gdy okno na ekranie nie jest kwadratem wymagane jest 
	// okreœlenie okna obserwatora. 
	// Pozwala to zachowaæ w³aœciwe proporcje rysowanego obiektu
	// Do okreœlenia okna obserwatora s³u¿y funkcja glOrtho(...)

	if (horizontal <= vertical)
		glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);
	else
		glOrtho(-100.0*AspectRatio, 100.0*AspectRatio, -100.0, 100.0, 1.0, -1.0);

	glMatrixMode(GL_MODELVIEW);
	// Okreœlenie uk³adu wspó³rzêdnych     
	glLoadIdentity();
}
/*************************************************************************************/

// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli


void main(void)
{
	cout << "Podaj glebokosc\n";
	cin >> glebokosc;
	/*cout << "Podaj przemieszczenie w %\n";
	cin >> przemieszczenie;
	if (przemieszczenie < 0)
		przemieszczenie = -przemieszczenie;*/

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	// Ustawienie trybu wyœwietlania
	// GLUT_SINGLE - pojedynczy bufor wyœwietlania
	// GLUT_RGBA - model kolorów RGB

	glutCreateWindow("Lab2 - 209865");
	// Utworzenie okna i okreœlenie treœci napisu w nag³ówku okna

	glutDisplayFunc(RenderScene);
	// Okreœlenie, ¿e funkcja RenderScene bêdzie funkcj¹ zwrotn¹ (callback)
	// Biblioteka GLUT bêdzie wywo³ywa³a t¹ funkcjê za ka¿dym razem, gdy
	// trzeba bêdzie przerysowaæ okno


	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za
	// zmiany rozmiaru okna

	MyInit();
	// Funkcja MyInit (zdefiniowana powy¿ej) wykonuje wszelkie  
	// inicjalizacje konieczneprzed przyst¹pieniem do renderowania

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT
}
/*************************************************************************************/