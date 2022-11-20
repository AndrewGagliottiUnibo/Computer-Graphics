#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

int submenu_Opzioni_I, menu_id, submenu_Hermite;
int mod_par_der = 0;  //(0) 1 : (non) si intende agire sui parametri T,B,C nel calcolo numerico della derivata nel vertice di controllo selezionato; 
int visualizzaTg = 0; //(0) 1 : (non) si intende visualizzare graficamente la tangente nei vertici di controllo
int visualizzaPC = 0; //(0) 1 : (non) si intende visualizzare graficamente il poligono di controllo
static unsigned int programId;
mat4 Projection,Model;
GLuint MatProj, MatModel;
int width = 1280;
int height = 720;

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint EBO_indici;
	int nTriangles;

	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	vector<vec4> colCP;
	vector<int> indici;

	int nv;

	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
} Figura;

Figura Poligonale;

void crea_VAO_CP(Figura* fig) {
	// I VAO in questo caso sono dinamici: le geometrie e i vertici cambiano
	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	// Genero , rendo attivo,  il VBO della geometria dei vertici di Controllo e lo inizializzo a NULL
	// Si alloca una memoria iniziale che poi sarà sottoposta a cambiamenti (riallocazione)
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->CP.size() * sizeof(vec3), NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, il VBO dei colori nei vertici di controllo e lo inizializzo a NULL
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colCP.size() * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void update_VAO_CP(Figura* fig) {
	//Rendo attivo il VAO
	glBindVertexArray(fig->VAO);
	// rendo attivo,aggiorno il VBO della geometria dei vertici di COntrollo
	
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->CP.size() * sizeof(vec3), fig->CP.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//  rendo attivo, aggiorno il VBO dei colori nei vertici di controllo
	 
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colCP.size() * sizeof(vec4), fig->colCP.data(), GL_DYNAMIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}
 
void INIT_SHADER(void) {
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_S.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {
	// Lo inizializza e basta, è tutto NULL
	crea_VAO_CP(&Poligonale);

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	 
	glViewport(0, 0, width, height);
}
 
void onMouse( int button,  int state, int x, int y) {

	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
		float mousex = x;
		// h - y per via del sistema di riferimento diverso tra quello del viewport e quello di vista
		float mousey = height - y;
	 

	Poligonale.CP.push_back(vec3(mousex, mousey, 0.0));
	Poligonale.colCP.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	}

	glutPostRedisplay();
}

void drawScene(void) {
	int i;
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	 
	// Si riempe il VAO dinamicamente
    update_VAO_CP(&Poligonale);

	Model = mat4(1.0);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
	glBindVertexArray(Poligonale.VAO);

	glDrawArrays(GL_POINTS, 0, Poligonale.CP.size());
	 
	glDrawArrays(GL_LINE_STRIP, 0, Poligonale.CP.size());
	glBindVertexArray(0);
	glutSwapBuffers();
}


void menu_M_I(int num) {
	//Menu per gestire la modalità di inserimento dei vertici della poligonale
	if (num == 1) {
	}

	if (num == 2) {
	}

	if (num == 3) {
	}

	if (num == 4) {
	}
	glutPostRedisplay();
}
void menu_Hermite(int num) {
	//Menu per la gestione delle curve di hermite
	if (num == 1) { 
	}

	if (num == 2) {
	}

	if (num == 3) {
	}

	if (num == 4) {
	}

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

	submenu_Hermite = glutCreateMenu(menu_Hermite);
	glutAddMenuEntry("Calcola Curva Interpolante", 0);
	glutAddMenuEntry("Attiva Modalita' modifica tangenti", 1);
	glutAddMenuEntry("Disattiva Modalita' modifica tangenti", 2);
	glutAddMenuEntry("Attiva Visualizzazione tangenti ", 3);
	glutAddMenuEntry("Disattiva Visualizzazione tangenti ", 4);

	menu_id = glutCreateMenu(menu);
	glutAddMenuEntry("Editor Curve", -1);
	glutAddSubMenu("Modalita inserimento", submenu_Opzioni_I);
	glutAddSubMenu("Curve cubiche di Hermite Interpolanti C1", submenu_Hermite);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Editor Curve");
	glutDisplayFunc(drawScene);
	glutMouseFunc(onMouse);
	glewExperimental = GL_TRUE;
	glewInit();		 
	INIT_SHADER();
	INIT_VAO();
	createMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}


