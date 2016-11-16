/*************************************************************************************/
// Adrian Frydmañski
// 209865
/*************************************************************************************/
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <stdio.h>
#include <math.h>

const float pi = 3.1415926535897932384626433832795;
const int n = 512;				// liczba punktów w kwadracie jednostkowym

typedef float point3[3];
point3 pTab[n][n];				// Deklaracja tablicy zawierajacej punkty
point3 lTab[n][n];				// Deklaracja tablicy opisuj¹cej oœwietlenie
point3 tTab[n][n];				// Deklaracja tablicy zawieraj¹cej wspó³rzêdne tekstury
static GLfloat thetaH = 0.0;	// k¹t obrotu obiektu (horizontal)
static GLfloat thetaV = 0.0;	// k¹t obrotu obiektu (vertical)
static GLfloat pix2angleH;		// przelicznik pikseli na stopnie (horizontal)
static GLfloat pix2angleV;		// przelicznik pikseli na stopnie (vertical)
// inicjalizacja po³o¿enia obserwatora
static GLfloat theta = 0.0;		// k¹t azymutu
static GLfloat phi = 0.0;		// k¹t elewacji
static GLfloat R = 16;			// promieñ sfery
static GLfloat errY = 1;
static GLint status = 0;		// stan klawiszy myszy: 0 - nie naciœniêto ¿adnego klawisza, 1 - naciœniêty zosta³ lewy klawisz
static int x_pos_old = 0;       // poprzednia pozycja kursora myszy (x)
static int y_pos_old = 0;		// poprzednia pozycja kursora myszy (y)
static int delta_x = 0;			// ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy 
static int delta_y = 0;			// ró¿nica pomiêdzy pozycj¹ bie¿¹c¹ i poprzedni¹ kursora myszy 
static float pos_z = 10.0;		// oddalenie obserwatora od jajka
static GLfloat viewer[] = { 0.0, 0.0, pos_z };	// inicjalizacja po³o¿enia obserwatora 
int mouse_mode = 2;				// Wybór trybu myszy: 1 - wyœwietlanie jajka, 2 - po³o¿enie obserwatora


/*************************************************************************************/
// Funkcja wczytuje dane obrazu zapisanego w formacie TGA w pliku o nazwie
GLbyte *LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat)
{
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
	// Wartoœci domyœlne zwracane w przypadku b³êdu
	*ImWidth = 0;
	*ImHeight = 0;
	*ImFormat = GL_BGR_EXT;
	*ImComponents = GL_RGB8;
	pFile = fopen(FileName, "rb");
	if (pFile == NULL) 
		return NULL;
	// Przeczytanie nag³ówka pliku
	fread(&tgaHeader, sizeof(TGAHEADER), 1, pFile);
	// Odczytanie szerokoœci, wysokoœci i g³êbi obrazu
	*ImWidth = tgaHeader.width;
	*ImHeight = tgaHeader.height;
	sDepth = tgaHeader.bitsperpixel / 8;
	// Sprawdzenie, czy g³êbia spe³nia za³o¿one warunki (8, 24, lub 32 bity)
	if (tgaHeader.bitsperpixel != 8 && tgaHeader.bitsperpixel != 24 && tgaHeader.bitsperpixel != 32) 
		return NULL;
	// Obliczenie rozmiaru bufora w pamiêci
	lImageSize = tgaHeader.width * tgaHeader.height * sDepth;
	// Alokacja pamiêci dla danych obrazu
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
// Funkcja generujaca chmurê punktow w kszta³cie jajka
void EggInit()
{
	// wype³nianie tablicy punktów
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
	// wype³nianie tablicy oœwietlenia
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
			// normalizacja wektorów
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
	// Czyszczenie okna aktualnym kolorem czyszcz¹cym
	glLoadIdentity();
	// Czyszczenie macierzy bie¿¹cej
	//gluLookAt(viewer[0],viewer[1],viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	// Zdefiniowanie po³o¿enia obserwatora
	if (status == 1)				// wciœniêty lewy przycisk myszy
	{
		// zmiana phi
		phi += delta_y*pix2angleH / 10.0;
		if (phi <= 0) phi += 2 * 3.14;
		if (phi >= 2 * 3.14) phi -= 2 * 3.14;
		// zmiana theta
		theta += delta_x*pix2angleV / 10.0;
		if (theta <= 0) theta += 2 * 3.14;
		if (theta >= 2 * 3.14) theta -= 2 * 3.14;
		// Sprawdzenie wartoœci k¹ta elewacji i modyfikacja
		// sk³adowej y wektora okreœlaj¹cego skrócenie kamery 
		// co zapewni p³ynny obrót obiektu w p³aszczyŸnie y
		if ((phi >= 0 && phi < 3.14 / 2) || (phi >= 3 * 3.14 / 2 && phi < 2 * 3.14)) errY = 1.0;
		else errY = -1.0;
	}
	if (status == 2)	// wciœniêty prawy przycisk myszy
	{
		R += delta_x;
		if (R <= 9.1) R = 9.0;
		if (R >= 25.1) R = 25.0;
	}
	// ustawienie po³o¿enia obserwatora
	viewer[0] = R*cos(theta)*cos(phi);
	viewer[1] = R*sin(phi);
	viewer[2] = R*sin(theta)*cos(phi);
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, errY, 0.0);
	//
	Egg();							// Rysowanie jajka
	glColor3f(1.0f, 1.0f, 1.0f);	// Ustawienie koloru rysowania na bia³y
	glFlush();						// Przekazanie poleceñ rysuj¹cych do wykonania
	glutSwapBuffers();
}

/*************************************************************************************/
// Funkcja przechwytuj¹ca stan myszy
void Mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		// przypisanie aktualnie odczytanej pozycji kursora jako pozycji poprzedniej
		x_pos_old = x;		
		y_pos_old = y;
		status = 1;         // wciœniêty zosta³ lewy klawisz myszy
	}
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		status = 2;
	}
	else status = 0;		// nie zosta³ wciœniêty ¿aden klawisz 
	glutPostRedisplay();
}


/*************************************************************************************/
// Funkcja "monitoruje" po³o¿enie kursora myszy i ustawia wartoœci odpowiednich zmiennych globalnych
void Motion(GLsizei x, GLsizei y)
{
	delta_x = x - x_pos_old;	// obliczenie ró¿nicy po³o¿enia kursora myszy
	x_pos_old = x;				// podstawienie bie¿acego po³o¿enia jako poprzednie
	delta_y = y - y_pos_old;	// obliczenie ró¿nicy po³o¿enia kursora myszy
	y_pos_old = y;				// podstawienie bie¿acego po³o¿enia jako poprzednie
	glutPostRedisplay();		// przerysowanie obrazu sceny
}

/*************************************************************************************/
// Funkcja inicjuj¹ca scen? do przerysowania
void MyInit(void)
{
	// Kolor czyszcz¹cy (wype³nienia okna) ustawiono na czarny
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Definicja materia³u z jakiego zrobione jest jajko
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };		// wspó³czynniki ka =[kar,kag,kab] dla œwiat³a otoczenia
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };		// wspó³czynniki kd =[kdr,kdg,kdb] dla œwiat³a rozproszonego
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// wspó³czynniki ks =[ksr,ksg,ksb] dla œwiat³a odbitego               
	GLfloat mat_shininess = { 20.0 };					// wspó³czynnik n opisuj¹cy po³ysk powierzchni

	// Definicja Ÿród³a œwiat³a
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };	// po³o¿enie Ÿród³a
	GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a otoczenia - Ia = [Iar,Iag,Iab]
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego - odbicie dyfuzyjne Id = [Idr,Idg,Idb]
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };	// sk³adowe intensywnoœci œwiecenia Ÿród³a œwiat³a powoduj¹cego odbicie kierunkowe Is = [Isr,Isg,Isb]
	GLfloat att_constant = { 1.0 };						// sk³adowa sta³a ds dla modelu zmian oœwietlenia w funkcji odleg³oœci od Ÿród³a
	GLfloat att_linear = { 0.05f };						// sk³adowa liniowa dl dla modelu zmian oœwietlenia w funkcji odleg³oœci od Ÿród³a
	GLfloat att_quadratic = { 0.001f };					// sk³adowa kwadratowa dq dla modelu zmian oœwietlenia w funkcji odleg³o?ci od Ÿród³a

	// Ustawienie patrametrów materia³u
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// Ustawienie parametrów Ÿród³a œwiat³a
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, att_constant);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, att_linear);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, att_quadratic);

	// Ustawienie opcji systemu oœwietlania sceny
	glShadeModel(GL_SMOOTH);							// w³aczenie ³agodnego cieniowania
	glEnable(GL_LIGHTING);								// w³aczenie systemu oœwietlenia sceny
	glEnable(GL_LIGHT0);								// w³¹czenie Ÿród³a o numerze 0
	glEnable(GL_DEPTH_TEST);							// w³¹czenie mechanizmu z-bufora

	// Zmienne dla obrazu tekstury
	GLbyte *pBytes;
	GLint ImWidth, ImHeight, ImComponents;
	GLenum ImFormat;
	//glEnable(GL_CULL_FACE);							// Teksturowanie prowadzone tyko po jednej stronie œciany
	pBytes = LoadTGAImage("teksturka3.tga", &ImWidth, &ImHeight, &ImComponents, &ImFormat);						//  Przeczytanie obrazu tekstury z pliku
	glTexImage2D(GL_TEXTURE_2D, 0, ImComponents, ImWidth, ImHeight, 0, ImFormat, GL_UNSIGNED_BYTE, pBytes);		// Zdefiniowanie tekstury 2-D
	free(pBytes);																								// zwolnienie pamiêci
	glEnable(GL_TEXTURE_2D);																					// W³¹czenie mechanizmu teksturowania
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);												// Ustalenie trybu teksturowania
	// Okreœlenie sposobu nak³adania tekstur
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
	glMatrixMode(GL_PROJECTION);											// Prze³¹czenie macierzy bie¿¹cej na macierz projekcji
	glLoadIdentity();														// Czyszcznie macierzy bie¿¹cej
	gluPerspective(70, 1.0, 1.0, 30.0);										// Ustawienie parametrów dla rzutu perspektywicznego

	// Ustawienie wielkoœci okna okna widoku (viewport) w zale¿noœci relacji pomiêdzy wysokoœci¹ i szerokoœci¹ okna
	if (horizontal <= vertical)
		glViewport(0, (vertical - horizontal) / 2, horizontal, horizontal);
	else
		glViewport((horizontal - vertical) / 2, 0, vertical, vertical);

	glMatrixMode(GL_MODELVIEW);												// Prze³¹czenie macierzy bie¿¹cej na macierz widoku modelu  
	glLoadIdentity();														// Czyszczenie macierzy bie¿¹cej
}

/*************************************************************************************/
// G³ówny punkt wejœcia programu
void main(void)
{
	// Ustawienie trybu wyswietlania GLUT_DOUBLE - podwójne buforowanie, GLUT_RGB - tryb RGB, GLUT_DEPTH - bufor g³êbokoœci
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Teksturowanie obiektów 3D - Adrian Frydmañski");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glEnable(GL_DEPTH_TEST);
	MyInit();
	glutMainLoop();
}
