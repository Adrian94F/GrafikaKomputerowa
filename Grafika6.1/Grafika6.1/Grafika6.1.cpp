/*************************************************************************************/
// Adrian Frydma�ski
// 209865
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
using namespace std;

typedef float point3[3];

/*************************************************************************************/
// Sta�e i zmienne globalne:
float squareLen = 1.0;									// d�ugo�� boku kwadratu jednostkowego
point3 v = { 0.05, 0.05, 0.05 };						// szybko�� obracania si�
static GLfloat theta[] = { 0.0, 0.0, 0.0 };
bool walls[5] = { true, true, true, true, true };
bool pt[2] = { true, false };

/*************************************************************************************/
// Rysowanie tr�jk�ta
void triangle()
{
	glColor3f(1, 1, 1);
	//rysowanie tr�jk�ta
	glBegin(GL_TRIANGLES);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-4, -2, 0);
		glNormal3f(0, 0, 1);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(4, -2, 0);
		glNormal3f(0, 0, 1);
		glTexCoord2f(0.5f, 1.0f);
		glVertex3f(0, 3, 0);
	glEnd();
}

/*************************************************************************************/
// Rysowanie ostros�upa
void pyramid()
{
	//ostros�up
	float v[5][3] = { { -5,-2,5 },{ 5,-2,5 },{ 5,-2,-5 },{ -5,-2,-5 },{ 0,5,0 } };
	float n[5][3] = { { 0, 0.55, 0.77 }, { 0.77, 0.55, 0 }, { 0, 0.55, -0.77 }, { -0.77, 0.55, 0 }, { 0, -1, 0 } };
	if (walls[0])
	{
		glBegin(GL_TRIANGLES);
			glNormal3fv(n[0]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(v[0]);
			glNormal3fv(n[0]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(v[1]);
			glNormal3fv(n[0]);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(v[4]);
		glEnd();
	}
		//
	if (walls[1])
	{
		glBegin(GL_TRIANGLES);
			glNormal3fv(n[1]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(v[1]);
			glNormal3fv(n[1]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(v[2]);
			glNormal3fv(n[1]);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(v[4]);
		glEnd();
	}
		//
	if (walls[2])
	{
		glBegin(GL_TRIANGLES);
			glNormal3fv(n[2]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(v[2]);
			glNormal3fv(n[2]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(v[3]);
			glNormal3fv(n[2]);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(v[4]);
		glEnd();
	}
		//
	if (walls[3])
	{
		glBegin(GL_TRIANGLES);
			glNormal3fv(n[3]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(v[3]);
			glNormal3fv(n[3]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(v[0]);
			glNormal3fv(n[3]);
			glTexCoord2f(0.5f, 1.0f);
			glVertex3fv(v[4]);
		glEnd();
	}
	//
	if (walls[4])
	{
		glBegin(GL_QUADS);
			glNormal3fv(n[4]);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3fv(v[0]);
			glNormal3fv(n[4]);
			glTexCoord2f(1.0f, 0.0f);
			glVertex3fv(v[3]);
			glNormal3fv(n[4]);
			glTexCoord2f(1.0f, 1.0f);
			glVertex3fv(v[2]);
			glNormal3fv(n[4]);
			glTexCoord2f(0.0f, 1.0f);
			glVertex3fv(v[1]);
		glEnd();
	}

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

	// zakomentowa� niepotrzebne
	if (pt[0]) pyramid();
	if (pt[1]) triangle();

	// Przekazanie polece� rysuj�cych do wykonania
	glFlush();

	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie 
// FileName, alokuje pami�� i zwraca wska�nik (pBits) do bufora w kt�rym 
// umieszczone s� dane. 
// Ponadto udost�pnia szeroko�� (ImWidth), wysoko�� (ImHeight) obrazu
// tekstury oraz dane opisuj�ce format obrazu wed�ug specyfikacji OpenGL 
// (ImComponents) i (ImFormat).
// Jest to bardzo uproszczona wersja funkcji wczytuj�cej dane z pliku TGA.
// Dzia�a tylko dla obraz�w wykorzystuj�cych 8, 24, or 32 bitowy kolor.
// Nie obs�uguje plik�w w formacie TGA kodowanych z kompresj� RLE.

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
	/*************************************************************************************/
	// Struktura dla nag��wka pliku  TGA

#pragma pack(1)            
	typedef struct
	{
		GLbyte    idlength;
		GLbyte    colormaptype;
		GLbyte    datatypecode;
		unsigned short    colormapstart;
		unsigned short    colormaplength;
		unsigned char     colormapdepth;
		unsigned short    x_orgin;
		unsigned short    y_orgin;
		unsigned short    width;
		unsigned short    height;
		GLbyte    bitsperpixel;
		GLbyte    descriptor;
	}TGAHEADER;
#pragma pack(8)


	FILE *pFile;
	TGAHEADER tgaHeader;
	unsigned long lImageSize;
	short sDepth;
	GLbyte    *pbitsperpixel = NULL;

	/*************************************************************************************/
	// Warto�ci domy�lne zwracane w przypadku b��du 

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;
	/*************************************************************************************/

	// Przeczytanie nag��wka pliku 

	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	/*************************************************************************************/
	// Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu 

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	/*************************************************************************************/

	// Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity) 

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/
	// Obliczenie rozmiaru bufora w pami�ci

	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	/*************************************************************************************/
	// Alokacja pami�ci dla danych obrazu

	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));

	if (pbitsperpixel == NULL)
		return NULL;

	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}

	/*************************************************************************************/
	// Ustawienie formatu OpenGL

	switch (sDepth)
	{
	case 3:
		*ImFormat = GL_BGR_EXT;
		*ImComponents = GL_RGB8;
		break;
	case 4:
		*ImFormat = GL_BGRA_EXT;
		*ImComponents = GL_RGBA8;
		break;
	case 1:
		*ImFormat = GL_LUMINANCE;
		*ImComponents = GL_LUMINANCE8;
		break;
	};

	fclose(pFile);

	return pbitsperpixel;
}

/*************************************************************************************/
// Funkcja zwrotna dla obrotu
void spin()
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
	// Zmienne dla obrazu tekstury
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	// Teksturowanie b�dzie prowadzone tyko po jednej stronie �ciany 
	glEnable(GL_CULL_FACE);

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	pBytes = LoadTGAImage("teksturka2.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	// Zdefiniowanie tekstury 2-D 
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	// Zwolnienie pami�ci
	free(pBytes);

	// W��czenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Okre�lenie sposobu nak�adania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);					// Kolor czyszc�cy (wype�nienia okna) ustawiono na czarny

															/*************************************************************************************/
															// Definicja materia�u z jakiego zrobiony jest czajnik 
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };			// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };			// wsp�czynniki kd =[kdr,kdg,kdb] �wiat�a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };		// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego                
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
// funkcja dla klawiatury
void keys(unsigned char key, int x, int y)
{
	if (key == '2') walls[0] = !walls[0];
	if (key == '6') walls[1] = !walls[1];
	if (key == '8') walls[2] = !walls[2];
	if (key == '4') walls[3] = !walls[3];
	if (key == '5') walls[4] = !walls[4];
	if (key == 'p') pt[0] = !pt[0];
	if (key == 't') pt[1] = !pt[1];

	RenderScene(); // przerysowanie obrazu sceny
}

/*************************************************************************************/
// G��wny punkt wej�cia programu. Program dzia�a w trybie konsoli
void main(void)
{
	// inicjowanie randa
	srand((unsigned)time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Tekstury");
	// Okre�lenie funkcji RenderScene jako funkcji zwrotnej (callback function)
	// B�dzie ona wywo�ywana za ka�dym razem, gdy zajdzie potrzba przeryswania okna
	glutDisplayFunc(RenderScene);
	// Dla aktualnego okna ustala funkcj� zwrotn� odpowiedzialn� za zmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);
	// Funkcja MyInit() wykonuje wszelkie inicjalizacje konieczne przed przyst�pieniem do renderowania 
	MyInit();
	// W��czenie mechanizmu usuwania powierzchni niewidocznych
	glEnable(GL_DEPTH_TEST);
	// Funkcja zwrotna obrotu i klawiszy
	glutIdleFunc(spin);
	glutKeyboardFunc(keys);
	// g��wna petla GLUTa
	glutMainLoop();
}