/*************************************************************************************/
// Adrian Frydmañski
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
// Sta³e i zmienne globalne:
float squareLen = 1.0;									// d³ugoœæ boku kwadratu jednostkowego
point3 v = { 0.05, 0.05, 0.05 };						// szybkoœæ obracania siê
static GLfloat theta[] = { 0.0, 0.0, 0.0 };
bool walls[5] = { true, true, true, true, true };
bool pt[2] = { true, false };

/*************************************************************************************/
// Rysowanie trójk¹ta
void triangle()
{
	glColor3f(1, 1, 1);
	//rysowanie trójk¹ta
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
// Rysowanie ostros³upa
void pyramid()
{
	//ostros³up
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

	// zakomentowaæ niepotrzebne
	if (pt[0]) pyramid();
	if (pt[1]) triangle();

	// Przekazanie poleceñ rysuj¹cych do wykonania
	glFlush();

	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie 
// FileName, alokuje pamiêæ i zwraca wskaŸnik (pBits) do bufora w którym 
// umieszczone s¹ dane. 
// Ponadto udostêpnia szerokoœæ (ImWidth), wysokoœæ (ImHeight) obrazu
// tekstury oraz dane opisuj¹ce format obrazu wed³ug specyfikacji OpenGL 
// (ImComponents) i (ImFormat).
// Jest to bardzo uproszczona wersja funkcji wczytuj¹cej dane z pliku TGA.
// Dzia³a tylko dla obrazów wykorzystuj¹cych 8, 24, or 32 bitowy kolor.
// Nie obs³uguje plików w formacie TGA kodowanych z kompresj¹ RLE.

GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
	/*************************************************************************************/
	// Struktura dla nag³ówka pliku  TGA

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
	// Wartoœci domyœlne zwracane w przypadku b³êdu 

	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;

	
	pFile = fopen(FileName, "rb");
	if (pFile == NULL)
		return NULL;
	/*************************************************************************************/

	// Przeczytanie nag³ówka pliku 

	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);

	/*************************************************************************************/
	// Odczytanie szerokoœci, wysokoœci i g³êbi obrazu 

	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;

	/*************************************************************************************/

	// Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity) 

	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32)
		return NULL;

	/*************************************************************************************/
	// Obliczenie rozmiaru bufora w pamiêci

	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;

	/*************************************************************************************/
	// Alokacja pamiêci dla danych obrazu

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

	glutPostRedisplay();								//odœwie¿enie zawartoœci okna
}

/*************************************************************************************/
// Funkcja ustalaj¹ca stan renderowania
void MyInit(void)
{
	// Zmienne dla obrazu tekstury
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;

	// Teksturowanie bêdzie prowadzone tyko po jednej stronie œciany 
	glEnable(GL_CULL_FACE);

	//  Przeczytanie obrazu tekstury z pliku o nazwie tekstura.tga
	pBytes = LoadTGAImage("teksturka2.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);

	// Zdefiniowanie tekstury 2-D 
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);

	// Zwolnienie pamiêci
	free(pBytes);

	// W³¹czenie mechanizmu teksturowania
	glEnable(GL_TEXTURE_2D);

	// Ustalenie trybu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Okreœlenie sposobu nak³adania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);					// Kolor czyszc¹cy (wype³nienia okna) ustawiono na czarny

															/*************************************************************************************/
															// Definicja materia³u z jakiego zrobiony jest czajnik 
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };			// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };			// wspó³czynniki kd =[kdr,kdg,kdb] œwiat³a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };		// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego                
	GLfloat mat_shininess = { 20.0 };						// wspó³czynnik n opisuj¹cy po³ysk powierzchni

															/*************************************************************************************/
															// Definicja Ÿród³a œwiat³a
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };		// po³o¿enie Ÿród³a
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };		// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia
															// Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };		// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
															// odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };		// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego
															// odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant = { 1.0 };							// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji 
															// odleg³oœci od Ÿród³a
	GLfloat att_linear = { 0.05f };							// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji 
															// odleg³oœci od Ÿród³a
	GLfloat att_quadratic = { 0.001f };						// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji
															// odleg³oœci od Ÿród³a

															/*************************************************************************************/
															// Ustawienie patrametrów materia³u
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	/*************************************************************************************/
	// Ustawienie parametrów Ÿród³a
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	/*************************************************************************************/
	// Ustawienie opcji systemu oœwietlania sceny 
	glShadeModel(GL_SMOOTH); // w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);   // w³aczenie systemu oœwietlenia sceny 
	glEnable(GL_LIGHT0);     // w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_DEPTH_TEST); // w³¹czenie mechanizmu z-bufora 
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
// G³ówny punkt wejœcia programu. Program dzia³a w trybie konsoli
void main(void)
{
	// inicjowanie randa
	srand((unsigned)time(NULL));
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Tekstury");
	// Okreœlenie funkcji RenderScene jako funkcji zwrotnej (callback function)
	// Bêdzie ona wywo³ywana za ka¿dym razem, gdy zajdzie potrzba przeryswania okna
	glutDisplayFunc(RenderScene);
	// Dla aktualnego okna ustala funkcjê zwrotn¹ odpowiedzialn¹ za zmiany rozmiaru okna 
	glutReshapeFunc(ChangeSize);
	// Funkcja MyInit() wykonuje wszelkie inicjalizacje konieczne przed przyst¹pieniem do renderowania 
	MyInit();
	// W³¹czenie mechanizmu usuwania powierzchni niewidocznych
	glEnable(GL_DEPTH_TEST);
	// Funkcja zwrotna obrotu i klawiszy
	glutIdleFunc(spin);
	glutKeyboardFunc(keys);
	// g³ówna petla GLUTa
	glutMainLoop();
}