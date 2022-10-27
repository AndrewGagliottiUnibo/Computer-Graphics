#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"

#define PI 3.14159265358979323846

// Per Curve di hermite
#define PHI0(t) (2.0 * t * t * t - 3.0 * t * t + 1)
#define PHI1(t) (t * t * t - 2.0 * t * t + t)
#define PSI0(t) (-2.0 * t * t * t + 3.0 * t * t)
#define PSI1(t) (t * t * t - t * t)

// viewport size
int width = 800;
int height = 800;
int selected_obj = -1;

// Inizializzazione Menu
int submenu_Opzioni_I, menu_id, submenu_SplineC1Interp, submenu_Bezier;

// Inizializzazioni
int mod_par_der = 0;  //(0) 1 : (non) si intende agire sui parametri T,B,C nel calcolo numerico della derivata nel vertice di controllo selezionato;
int visualizzaTg = 0; //(0) 1 : (non) si intende visualizzare graficamente la tangente nei vertici di controllo
int visualizzaPC = 0; //(0) 1 : (non) si intende visualizzare graficamente il poligono di controllo

// Inizializzazione parametri Tes, Bias, Cont per la modifica delle derivate agli estremi
float Tens = 0.0, Bias = 0.0, Cont = 0.0; // Questa inizializzazione 'rappresenta le derivate come semplice rapporto incrementale

int metodo = 1; // Metodo=1 --> interpolazione curve di Hermite;  Metodo=2 --> approssimazione di forma curva di Bezier

int M_I = 0;
static unsigned int programId;
mat4 Projection;
GLuint MatProj, MatModel;
unsigned int lsceltavs, lsceltafs, loc_time, loc_res, loc_mouse;
vec2 res, mouse;

int pval = 140;
float *t;

typedef struct
{
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint EBO_indici;
	int nTriangles;
	// Vertici
	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	vector<vec4> colCP;
	vector<int> indici;
	// Numero vertici
	int nv;
	// Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	int sceltaVS;
	int sceltaFS;
	string name;
} Figura;

Figura Curva, Poligonale, Derivata, tangenti;

void crea_VAO_Vector(Figura *fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	// Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3), fig->vertici.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	// Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);
}

void crea_VAO_CP(Figura *fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	// Genero, rendo attivo, riempio il VBO della geometria dei vertici di Controllo
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->CP.size() * sizeof(vec3), fig->CP.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero, rendo attivo, riempio il VBO dei colori nei vertici di controllo
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colCP.size() * sizeof(vec4), fig->colCP.data(), GL_DYNAMIC_DRAW);
	// Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);
}

/////////////////////////////////////// Disegna geometria ///////////////////////////////////////////
float dx(int i, float *t, float Tens, float Bias, float Cont, Figura *Fig) {
	if (i == 0)
		return 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i + 1].x - Fig->CP[i].x) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i].x - Fig->CP[i - 1].x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
	else
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
}
float dy(int i, float *t, float Tens, float Bias, float Cont, Figura *Fig) {
	if (i == 0)
		return 0.5 * (1.0 - Tens) * (1.0 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	else
		return 0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
}

float DX(int i, float *t) {
	// Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente x della curva � quello originale, altrimenti � quello che � stato modificato nella funzione
	// keyboardfunc in seguito alla modifica dei valori Tens, Bias e Cont.
	if (Derivata.CP.at(i).x == 0)
		return dx(i, t, 0.0, 0.0, 0.0, &Poligonale);

	if (Derivata.CP.at(i).x != 0)
		return Derivata.CP.at(i).x;
}

float DY(int i, float *t) {
	// Nei vertici di controllo per i quali non sono stati modificati i parametri Tens, Bias, Cont il valore della derivata della componente y della curva � quello originale, altrimenti � quello che � stato modificato nella funzione
	// keyboardfunc  in seguito alla modifica dei valori Tens, Bias e Cont.
	if (Derivata.CP.at(i).y == 0)
		return dy(i, t, 0.0, 0.0, 0.0, &Poligonale);

	if (Derivata.CP.at(i).y != 0)
		return Derivata.CP.at(i).y;
}

void InterpolazioneHermite(float *t, Figura *Fig, vec4 color_top, vec4 color_bot) {
	float p_t = 0, p_b = 0, p_c = 0, x, y;
	float passotg = 1.0 / (float)(pval - 1);

	float tg = 0, tgmapp, ampiezza;
	int i = 0;
	int is = 0; // indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg appartiene

	Fig->vertici.clear();
	Fig->colors.clear();
	tangenti.CP.clear();

	for (tg = 0; tg <= 1; tg += passotg) {
		// Localizzare l'intervallo [t[is], t[is + 1] a cui tg appartiene]
		if (tg > t[is + 1]) {
			is++;
		}

		// Mappo tg in [0, 1]
		// tg è sottoposto a trasformazione affine che lo mappa in [t[is], tt[is + 1]
		float ampiezza = t[is + 1] - t[is];
		float tgmapp = (tg - t[is]) / ampiezza;

		// E questi sono i contributi dei termini da conservare nei vertici da poi disegnare
		x = Fig->CP[is].x * PHI0(tgmapp) + DX(is, t) * PHI1(tgmapp) * ampiezza + 
					Fig->CP[is + 1].x * PSI0(tgmapp) + DX(is + 1, t) * PSI1(tgmapp) * ampiezza;
		y = Fig->CP[is].y * PHI0(tgmapp) + DX(is, t) * PHI1(tgmapp) * ampiezza + 
					Fig->CP[is + 1].y * PSI0(tgmapp) + DX(is + 1, t) * PSI1(tgmapp) * ampiezza;

		Fig->vertici.push_back(vec3(x, y, 0.0));
		Fig->colors.push_back(color_top);
	}

	// Rette tangenti ai vertici di controllo - TODO
	//if (visualizzaTg == 1)
	//{
	//}
}

//void mouseMotion(int x, int y) {
//
//	float mousex = x;
//	float mousey = height - y;
//
//	if (M_I == 1)
//	{
//		if (selected_obj >= 0)
//		{
//		}
//	}
//	glutPostRedisplay();
//}

void Mouse(int button, int state, int x, int y) {
	int i;
	float dist, dist1;
	float TOLL = 10; // Tolleranza per la selezione del vertice di controllo da modificare

	if (state == GLUT_DOWN)	{
		switch (button)	{

		case GLUT_LEFT_BUTTON:
			float mousex = (float)x;
			float mousey = (float)height - y;
			if (M_I == 1 || mod_par_der == 1) {
				// TODO
			}
			else if (M_I == 0) {
				Curva.CP.push_back(vec3(mousex, mousey, 0.0));
				Curva.colCP.push_back(vec4(1.0, 0.0, 0.0, 1.0));
				// Ad ogni vertice di controllo associo un valore vec3(0.0,0.0,0.0) per indicare che la derivata in quel vertice di controllo non e' stata modificata
				Derivata.CP.push_back(vec3(0.0, 0.0, 0.0));
			}
			else if (M_I == 2) {
				// Elimino l'ultimo vertice di controllo introdotto.
				Curva.CP.pop_back();
				Curva.colCP.pop_back();
			}
		}
	}

	glutPostRedisplay();
}

void mykeyboard(unsigned char key, int x, int y) {
	// TODO
	switch (key)
	{
	case 'T':

		break;
	case 't':

		break;

	case 'C':

		break;
	case 'c':

		break;
	case 'B':

		break;
	case 'b':

		break;
	}

	// Modifico la derivata del vertice selezionato con valori Tens, cont e Bias selezionati da tastiera
	// TODO

	glutPostRedisplay();
}

void costruisci_formaHermite(vec4 color_top, vec4 color_bot, Figura *forma) {

	Poligonale.CP = Curva.CP;
	Poligonale.colCP = Curva.colCP;

	if (Poligonale.CP.size() > 1)
	{
		// n + 1 parametri equidistanti nell'intervallo [0, 1]
		t = new float[Curva.CP.size()];
		int i;
		float step = 1.0 / (float)(Curva.CP.size() - 1);

		for (i = 0; i < Curva.CP.size(); i++)
			t[i] = (float)i * step;

		// Viene chiamata per processare le nuove elaborazioni dei vertici
		InterpolazioneHermite(t, &Curva, color_top, color_bot);

		forma->nv = Curva.vertici.size();
	}
}

void INIT_VAO(void) {
	Curva.sceltaFS = 0;
	Curva.sceltaVS = 0;
	Curva.name = "Curva";

	Poligonale.sceltaFS = 0;
	Poligonale.sceltaVS = 0;
	Poligonale.name = "Poligonale";

	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	lsceltavs = glGetUniformLocation(programId, "sceltaVS");
	lsceltafs = glGetUniformLocation(programId, "sceltaFS");

	loc_time = glGetUniformLocation(programId, "time");
	loc_res = glGetUniformLocation(programId, "res");
	loc_mouse = glGetUniformLocation(programId, "mouse");

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
}

void INIT_SHADER(void) {
	GLenum ErrorCheckValue = glGetError();

	char *vertexShader = (char *)"vertexShader_M.glsl";
	char *fragmentShader = (char *)"fragmentShader_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void resize(GLsizei w, GLsizei h) {

	float AspectRatio_mondo = (float)(width) / (float)(height);

	// Se l'aspect ratio del mondo e' diversa da quella della finestra devo mappare in modo diverso per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h) {
		glViewport(0, 0, w, w / AspectRatio_mondo);
	}
	else {
		glViewport(0, 0, h * AspectRatio_mondo, h);
	}

	glutPostRedisplay();
}

void drawScene(void) {
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	vec4 col_bottom = vec4{0.5451, 0.2706, 0.0745, 1.0000};
	vec4 col_top = vec4{1.0, 0.4980, 0.0353, 1.0000};

	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	// Caso metodo=1 : costrusci curva di Hermite
	if (metodo == 1) {
		costruisci_formaHermite(col_top, col_bottom, &Curva);
		crea_VAO_Vector(&Curva);
		crea_VAO_CP(&Poligonale);
	}

	glUniform2f(loc_res, res.x, res.y);
	glUniform2f(loc_mouse, mouse.x, mouse.y);
	glUniform1i(lsceltavs, Curva.sceltaVS);
	glUniform1i(lsceltafs, Curva.sceltaFS);
	// la matrice di Modellazione della Curva coincide con la matrice identit�, perch� selezioniamo i punti con il mouse in coordinate del mondo
	Curva.Model = mat4(1.0);
	glPointSize(6.0);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Curva.Model));

	// Disegno la curca se ho piu' di 2 vertici di controllo
	if (Poligonale.CP.size() > 1) {
		glBindVertexArray(Curva.VAO);
		glDrawArrays(GL_LINE_STRIP, 0, Curva.vertici.size());
		glBindVertexArray(0);
	}

	// Disegno i vertici di controllo
	glBindVertexArray(Poligonale.VAO);
	glDrawArrays(GL_POINTS, 0, Poligonale.CP.size());
	glBindVertexArray(0);

	// Se ho selezionato la costruzione della curva interpolante di Hermite, visualizzo le tangenti se visualizzaTg=1
	if (metodo == 1) {
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Curva.Model));
		if (visualizzaTg == 1) {
			crea_VAO_CP(&tangenti);
			glBindVertexArray(tangenti.VAO);
			glDrawArrays(GL_LINES, 0, tangenti.CP.size());
			glBindVertexArray(0);
		}
	}

	// Se ho selezionato la costruzione della curva interpolante di Bezier, visualizzo il poligono di controllo se visualizzaPC=1
	if (metodo == 2) {
		// TODO
	}

	glutSwapBuffers();
}

void menu_Bezier(int num) {
	metodo = 2;
	if (num == 1)
		visualizzaPC = 1;
	if (num == 2)
		visualizzaPC = 0;

	glutPostRedisplay();
}

void menu_splineInterpC1(int num) {
	metodo = 1;

	if (num == 1) {
		mod_par_der = 1;
		M_I = -1;
	}

	if (num == 2)
		mod_par_der = 0;

	if (num == 3)
		visualizzaTg = 1;

	if (num == 4)
		visualizzaTg = 0;

	glutPostRedisplay();
}

void menu_M_I(int num) {
	M_I = num;
	if (M_I == 1)
		selected_obj = -1;

	glutPostRedisplay();
}

void menu(int num) {
	glutPostRedisplay();
}

void createMenu(void) {
	// Creazione sottomenu da attaccare all'item del menu relativo alla scelta
	// del tipo di inserimento
	submenu_Opzioni_I = glutCreateMenu(menu_M_I);
	glutAddMenuEntry("Inserisci", 0);
	glutAddMenuEntry("Sposta", 1);
	glutAddMenuEntry("Elimina", 2);

	submenu_SplineC1Interp = glutCreateMenu(menu_splineInterpC1);
	glutAddMenuEntry("Calcola Curva Interpolante", 0);
	glutAddMenuEntry("Attiva Modalita' modifica tangenti", 1);
	glutAddMenuEntry("Disattiva Modalita' modifica tangenti", 2);
	glutAddMenuEntry("Attiva Visualizzazione tangenti ", 3);
	glutAddMenuEntry("Disattiva Visualizzazione tangenti ", 4);

	submenu_Bezier = glutCreateMenu(menu_Bezier);
	glutAddMenuEntry("Calcola Curva Approssimante di Forma", 0);
	glutAddMenuEntry("Attiva Visualizzazione Poligono di Controllo ", 1);
	glutAddMenuEntry("Disattiva Visualizzazione Poligono di controllo ", 2);

	// Creazione del menu prinicpale
	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Editor Curve", -1);
	glutAddSubMenu("Modalita inserimento", submenu_Opzioni_I);
	glutAddSubMenu("Curve cubiche di Hermite Interpolanti C1", submenu_SplineC1Interp);
	glutAddSubMenu("Curve di Bezier", submenu_Bezier);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Curve OpenGL");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseFunc(Mouse);
	glutKeyboardFunc(mykeyboard);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	createMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glutMotionFunc(mouseMotion);

	glutMainLoop();
}