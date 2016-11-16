/*************************************************************************************/
// Adrian Frydma�ski
// 209865
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <math.h>

const float pi = 3.1415926535897932384626433832795;
const int n = 512;				// liczba punkt�w w kwadracie jednostkowym

typedef float point3[3];
point3 pTab[n][n];				// Deklaracja tablicy zawierajacej punkty
point3 lTab[n][n];				// Deklaracja tablicy opisuj�cej o�wietlenie
point3 tTab[n][n];				// Deklaracja tablicy zawieraj�cej wsp�rz�dne tekstury
static GLfloat thetaH = 0.0;	// k�t obrotu obiektu (horizontal)
static GLfloat thetaV = 0.0;	// k�t obrotu obiektu (vertical)
static GLfloat pix2angleH;		// przelicznik pikseli na stopnie (horizontal)
static GLfloat pix2angleV;		// przelicznik pikseli na stopnie (vertical)
// inicjalizacja po�o�enia obserwatora
static GLfloat theta = 0.0;		// k�t azymutu
static GLfloat phi = 0.0;		// k�t elewacji
static GLfloat R = 16;			// promie� sfery
static GLfloat errY = 1;
static GLint status = 0;		// stan klawiszy myszy: 0 - nie naci�ni�to �adnego klawisza, 1 - naci�ni�ty zosta� lewy klawisz
static int x_pos_old = 0;       // poprzednia pozycja kursora myszy (x)
static int y_pos_old = 0;		// poprzednia pozycja kursora myszy (y)
static int delta_x = 0;			// r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy 
static int delta_y = 0;			// r�nica pomi�dzy pozycj� bie��c� i poprzedni� kursora myszy 
static float pos_z = 10.0;		// oddalenie obserwatora od jajka
static GLfloat viewer[] = { 0.0, 0.0, pos_z };	// inicjalizacja po�o�enia obserwatora 
int mouse_mode = 2;				// Wyb�r trybu myszy: 1 - wy�wietlanie jajka, 2 - po�o�enie obserwatora


/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
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
	// Warto�ci domy�lne zwracane w przypadku b��du
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	pFile = fopen(FileName, "rb");
	if (pFile == NULL) 
		return NULL;
	// Przeczytanie nag��wka pliku
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
	// Odczytanie szeroko�ci, wysoko�ci i g��bi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	// Sprawdzenie, czy g��bia spe�nia za�o�one warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32) 
		return NULL;
	// Obliczenie rozmiaru bufora w pami�ci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
	// Alokacja pami�ci dla danych obrazu
	pbitsperpixel = (GLbyte*)malloc(lImageSize * sizeof(GLbyte));
	if (pbitsperpixel == NULL) 
		return NULL;
	if (fread(pbitsperpixel, lImageSize, 1, pFile) != 1)
	{
		free(pbitsperpixel);
		return NULL;
	}
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
// Funkcja generujaca chmur� punktow w kszta�cie jajka
void EggInit()
{
	// wype�nianie tablicy punkt�w
	for (int i = 0; i<n; i++)
	{
		for (int k = 0; k<n; k++)
		{
			float u = (float)i / (n - 1);
			float v = (float)k / (n - 1);
			pTab[i][k][0] = (-90.0*u*u*u*u*u + 225.0*u*u*u*u - 270.0*u*u*u + 180.0*u*u - 45.0*u)*cos(pi*v);
			pTab[i][k][1] = (160.0*u*u*u*u - 320.0*u*u*u + 160.0*u*u) - 5.0;
			pTab[i][k][2] = (-90.0*u*u*u*u*u + 225.0*u*u*u*u - 270.0*u*u*u + 180.0*u*u - 45.0*u)*sin(pi*v);
		}
	}
	// wype�nianie tablicy o�wietlenia
	for (int i = 0; i<n; i++)
	{
		for (int k = 0; k<n; k++)
		{
			float u = (float)i / (n - 1);
			float v = (float)k / (n - 1);
			tTab[i][k][1] = u;
			tTab[i][k][0] = 1 - v;
			//
			float xu = (-450 * u*u*u*u + 900 * u*u*u - 810 * u*u + 360 * u - 45)*cos(pi*v);
			float xv = pi*(90 * u*u*u*u*u - 225 * u*u*u*u + 270 * u*u*u - 180 * u*u + 45 * u)*sin(pi*v);
			float yu = 640 * u*u*u - 960 * u*u + 320 * u;
			float yv = 0;
			float zu = (-450 * u*u*u*u + 900 * u*u*u - 810 * u*u + 360 * u - 45)*sin(pi*v);
			float zv = -pi*(90 * u*u*u*u*u - 225 * u*u*u*u + 270 * u*u*u - 180 * u*u + 45 * u)*cos(pi*v);
			//
			float x = (GLfloat)(yu*zv - zu*yv);
			float y = (GLfloat)(zu*xv - xu*zv);
			float z = (GLfloat)(xu*yv - yu*xv);
			// normalizacja wektor�w
			if (i < n / 2)
			{
				lTab[i][k][0] = x / (float)sqrt(x*x + y*y + z*z);
				lTab[i][k][1] = y / (float)sqrt(x*x + y*y + z*z);
				lTab[i][k][2] = z / (float)sqrt(x*x + y*y + z*z);
			}
			if (i >= n / 2)
			{
				lTab[i][k][0] = (-1)*x / (float)sqrt(x*x + y*y + z*z);
				lTab[i][k][1] = (-1)*y / (float)sqrt(x*x + y*y + z*z);
				lTab[i][k][2] = (-1)*z / (float)sqrt(x*x + y*y + z*z);
			}
			if (i == 0 || i == n - 1)
			{
				lTab[i][k][0] = 0;
				lTab[i][k][1] = -1;
				lTab[i][k][2] = 0;
			}
		}
	}
}
void Egg()
{
	EggInit();
	// rysowanie
	for (int i = 0; i<n; i++)
	{
		for (int k = 0; k<n; k++)
		{
			if (i < n - 1 && k < n - 1)
			{
				if (i < n / 2)
				{
					glBegin(GL_TRIANGLES);

					glNormal3fv(lTab[i][k]);
					glTexCoord2f(tTab[i][k][0], tTab[i][k][1]);
					glVertex3fv(pTab[i][k]);

					glNormal3fv(lTab[i + 1][k]);
					glTexCoord2f(tTab[i + 1][k][0], tTab[i + 1][k][1]);
					glVertex3fv(pTab[i + 1][k]);

					glNormal3fv(lTab[i + 1][k + 1]);
					glTexCoord2f(tTab[i + 1][k + 1][0], tTab[i + 1][k + 1][1]);
					glVertex3fv(pTab[i + 1][k + 1]);

					glNormal3fv(lTab[i][k]);
					glTexCoord2f(tTab[i][k][0], tTab[i][k][1]);
					glVertex3fv(pTab[i][k]);

					glNormal3fv(lTab[i + 1][k + 1]);
					glTexCoord2f(tTab[i + 1][k + 1][0], tTab[i + 1][k + 1][1]);
					glVertex3fv(pTab[i + 1][k + 1]);

					glNormal3fv(lTab[i][k + 1]);
					glTexCoord2f(tTab[i][k + 1][0], tTab[i][k + 1][1]);
					glVertex3fv(pTab[i][k + 1]);

					glEnd();
				}
				else
				{
					glBegin(GL_TRIANGLES);

					glNormal3fv(lTab[i][k]);
					glTexCoord2f(tTab[i][k][0], tTab[i][k][1]);
					glVertex3fv(pTab[i][k]);

					glNormal3fv(lTab[i + 1][k]);
					glTexCoord2f(tTab[i + 1][k][0], tTab[i + 1][k][1]);
					glVertex3fv(pTab[i + 1][k]);

					glNormal3fv(lTab[i + 1][k + 1]);
					glTexCoord2f(tTab[i + 1][k + 1][0], tTab[i + 1][k + 1][1]);
					glVertex3fv(pTab[i + 1][k + 1]);

					glNormal3fv(lTab[i][k]);
					glTexCoord2f(tTab[i][k][0], tTab[i][k][1]);
					glVertex3fv(pTab[i][k]);

					glNormal3fv(lTab[i][k + 1]);
					glTexCoord2f(tTab[i][k + 1][0], tTab[i][k + 1][1]);
					glVertex3fv(pTab[i][k + 1]);

					glNormal3fv(lTab[i + 1][k + 1]);
					glTexCoord2f(tTab[i + 1][k + 1][0], tTab[i + 1][k + 1][1]);
					glVertex3fv(pTab[i + 1][k + 1]);

					glEnd();
				}
			}
		}
	}
}

/*************************************************************************************/
// Funkcja okreslajaca co ma byc rysowane (zawsze wywolywana gdy trzeba przerysowac scene)
void RenderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszcz�cym
	glLoadIdentity();
	// Czyszczenie macierzy bie��cej
	//gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po�o�enia obserwatora
	if (status == 1)				// wci�ni�ty lewy przycisk myszy
	{
		// zmiana phi
		phi += delta_y*pix2angleH / 10.0;
		if (phi <= 0) phi += 2 * 3.14;
		if (phi >= 2 * 3.14) phi -= 2 * 3.14;
		// zmiana theta
		theta += delta_x*pix2angleV / 10.0;
		if (theta <= 0) theta += 2 * 3.14;
		if (theta >= 2 * 3.14) theta -= 2 * 3.14;
		// Sprawdzenie warto�ci k�ta elewacji i modyfikacja
		// sk�adowej y wektora okre�laj�cego skr�cenie kamery 
		// co zapewni p�ynny obr�t obiektu w p�aszczy�nie y
		if ((phi >= 0 && phi < 3.14 / 2) || (phi >= 3 * 3.14 / 2 && phi < 2 * 3.14)) errY = 1.0;
		else errY = -1.0;
	}
	if (status == 2)	// wci�ni�ty prawy przycisk myszy
	{
		R += delta_x;
		if (R <= 9.1) R = 9.0;
		if (R >= 25.1) R = 25.0;
	}
	// ustawienie po�o�enia obserwatora
	viewer[0] = R*cos(theta)*cos(phi);
	viewer[1] = R*sin(phi);
	viewer[2] = R*sin(theta)*cos(phi);
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, errY, 0.0);
	//
	Egg();							// Rysowanie jajka
	glColor3f(1.0f, 1.0f, 1.0f);	// Ustawienie koloru rysowania na bia�y
	glFlush();						// Przekazanie polece� rysuj�cych do wykonania
	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja przechwytuj�ca stan myszy
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		x_pos_old = x;		
		y_pos_old = y;
		status = 1;         // wci�ni�ty zosta� lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		status = 2;
	}
	else status = 0;		// nie zosta� wci�ni�ty �aden klawisz 
	glutPostRedisplay();
}


/*************************************************************************************/
// Funkcja "monitoruje" po�o�enie kursora myszy i ustawia warto�ci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;	// obliczenie r�nicy po�o�enia kursora myszy
	x_pos_old = x;				// podstawienie bie�acego po�o�enia jako poprzednie
	delta_y = y - y_pos_old;	// obliczenie r�nicy po�o�enia kursora myszy
	y_pos_old = y;				// podstawienie bie�acego po�o�enia jako poprzednie
	glutPostRedisplay();		// przerysowanie obrazu sceny
}

/*************************************************************************************/
// Funkcja inicjuj�ca scen? do przerysowania
void MyInit(void)
{
	// Kolor czyszcz�cy (wype�nienia okna) ustawiono na czarny
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Definicja materia�u z jakiego zrobione jest jajko
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };		// wsp�czynniki ka =[kar,kag,kab] dla �wiat�a otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };		// wsp�czynniki kd =[kdr,kdg,kdb] dla �wiat�a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// wsp�czynniki ks =[ksr,ksg,ksb] dla �wiat�a odbitego               
	GLfloat mat_shininess = { 20.0 };					// wsp�czynnik n opisuj�cy po�ysk powierzchni

	// Definicja �r�d�a �wiat�a
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };	// po�o�enie �r�d�a
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a otoczenia - Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego - odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// sk�adowe intensywno�ci �wiecenia �r�d�a �wiat�a powoduj�cego odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant = { 1.0 };						// sk�adowa sta�a ds dla modelu zmian o�wietlenia w funkcji odleg�o�ci od �r�d�a
	GLfloat att_linear = { 0.05f };						// sk�adowa liniowa dl dla modelu zmian o�wietlenia w funkcji odleg�o�ci od �r�d�a
	GLfloat att_quadratic = { 0.001f };					// sk�adowa kwadratowa dq dla modelu zmian o�wietlenia w funkcji odleg�o?ci od �r�d�a

	// Ustawienie patrametr�w materia�u
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Ustawienie parametr�w �r�d�a �wiat�a
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	// Ustawienie opcji systemu o�wietlania sceny
	glShadeModel(GL_SMOOTH);							// w�aczenie �agodnego cieniowania
	glEnable(GL_LIGHTING);								// w�aczenie systemu o�wietlenia sceny
	glEnable(GL_LIGHT0);								// w��czenie �r�d�a o numerze 0
	glEnable(GL_DEPTH_TEST);							// w��czenie mechanizmu z-bufora

	// Zmienne dla obrazu tekstury
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	//glEnable(GL_CULL_FACE);							// Teksturowanie prowadzone tyko po jednej stronie �ciany
	pBytes = LoadTGAImage("teksturka3.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);						//  Przeczytanie obrazu tekstury z pliku
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);		// Zdefiniowanie tekstury 2-D
	free(pBytes);																								// zwolnienie pami�ci
	glEnable(GL_TEXTURE_2D);																					// W��czenie mechanizmu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);												// Ustalenie trybu teksturowania
	// Okre�lenie sposobu nak�adania tekstur
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

/*************************************************************************************/
// Funkcja ma za zadanie utrzymanie stalych proporcji rysowanych w przypadku zmiany rozmiarow okna.
// Parametry vertical i horizontal (wysokosc i szerokosc okna) sa przekazywane do funkcji za kazdym razem gdy zmieni sie rozmiar okna.
void ChangeSize(GLsizei horizontal, GLsizei vertical)
{
	pix2angleH = 360.0 / (float)horizontal;									// przeliczenie pikseli na stopnie
	pix2angleV = 360.0 / (float)vertical;									// przeliczenie pikseli na stopnie
	glMatrixMode(GL_PROJECTION);											// Prze��czenie macierzy bie��cej na macierz projekcji
	glLoadIdentity();														// Czyszcznie macierzy bie��cej
	gluPerspective(70, 1.0, 1.0, 30.0);										// Ustawienie parametr�w dla rzutu perspektywicznego

	// Ustawienie wielko�ci okna okna widoku (viewport) w zale�no�ci relacji pomi�dzy wysoko�ci� i szeroko�ci� okna
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

	glMatrixMode(GL_MODELVIEW);												// Prze��czenie macierzy bie��cej na macierz widoku modelu  
	glLoadIdentity();														// Czyszczenie macierzy bie��cej
}

/*************************************************************************************/
// G��wny punkt wej�cia programu
void main(void)
{
	// Ustawienie trybu wyswietlania GLUT_DOUBLE - podw�jne buforowanie, GLUT_RGB - tryb RGB, GLUT_DEPTH - bufor g��boko�ci
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Teksturowanie obiekt�w 3D - Adrian Frydma�ski");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glEnable(GL_DEPTH_TEST);
	MyInit();
	glutMainLoop();
}
