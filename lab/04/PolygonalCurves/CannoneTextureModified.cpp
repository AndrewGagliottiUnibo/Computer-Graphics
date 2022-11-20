#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace glm;
static unsigned int programId;

#define  PI   3.14159265358979323846

float angolo = 0.0f;
float s = 1, dx = 0, dy = 0;
float dfx = 0, dfy = 0;
mat4 Projection;
GLuint MatProj, MatModel, locTime, locRes;
float posx_Proiettile, posy_Proiettile;
int nv_P;

// viewport size
int width = 1280;
int height = 720;
int posx = (float)width / 2.0, posy = (float)height / 4.0;

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;
	vector<vec3> vertici;
	vector<vec4> colors;
	int nv;
	int nVertici;
	mat4 Model;
	vec3 corner_b;
	vec3 corner_t;
} Figura;

vector<Figura> Scena;

Figura Farf = {};
Figura Cannone = {};
Figura Proiettile = {};

void updateProiettile(int value) {
	//Ascissa del proiettile durante lo sparo
	posx_Proiettile = 0;
	//Ordinata del proettile durante lo sparo
	posy_Proiettile += 4;

	//L'animazione deve avvenire finchè  l'ordinata del proiettile raggiunge un certo valore fissato
	if (posy_Proiettile <= 800) {
		glutTimerFunc(5, updateProiettile, 0);
	}
	else {
		posy_Proiettile = 0;
	}

	glutPostRedisplay();
}

void crea_VAO_Vector(Figura* fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3), fig->vertici.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}


void costruisci_cannone(float cx, float cy, float raggiox, float raggioy, Figura* fig) {

	int i;
	float stepA = (PI) / fig->nTriangles;
	float t;

	fig->vertici.push_back(vec3(cx, cy, 0.0));
	fig->colors.push_back(vec4(255.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA + PI;

		fig->vertici.push_back(vec3(cx + raggiox * cos(t), cy + raggioy * sin(t), 0.0));
		fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	}

	//Primo trapezio
	fig->vertici.push_back(vec3(-1.0, 0.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(1.0, 0.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(0.5, 2.5, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(0.5, 2.5, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(-1.0, 0.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(-0.5, 2.5, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));

	//Secondo trapezio al contrario
	fig->vertici.push_back(vec3(0.5, 2.5, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(1.0, 3.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(-0.5, 2.5, 0.0));
	fig->vertici.push_back(vec3(1.0, 3.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(-1.0, 3.0, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(-0.5, 2.5, 0.0));
	fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));

	fig->nVertici = fig->vertici.size();
}

void costruisci_proiettile(float cx, float cy, float raggiox, float raggioy, Figura* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;

	fig->vertici.push_back(vec3(cx, cy, 0.0));
	fig->colors.push_back(vec4(255.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA;

		fig->vertici.push_back(vec3(cx + raggiox * cos(t), cy + raggioy * sin(t), 0.0));
		fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	}

	fig->nVertici = fig->vertici.size();

	int minX;
	int maxX;
	int minY;
	int maxY;
	for (int i = 0; i < fig->nVertici; i++) {

	}
	fig->corner_t = vec3(-1.0, -1.0, 0.0);
	fig->corner_b = vec3(1.0, 1.0, 0.0);
}

void costruisci_farfalla(float cx, float cy, float raggiox, float raggioy, Figura* fig) {
	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;


	fig->vertici.push_back(vec3(cx, cy, 0.0));
	fig->colors.push_back(vec4(150.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA;
		fig->vertici.push_back(vec3(cx + raggiox * (sin(t) * (exp(cos(t)) - 2 * cos(4 * t)) + pow(sin(t / 12), 5)) / 4, cy + raggioy * (cos(t) * (exp(cos(t)) - 2 * cos(4 * t)) + pow(sin(t / 12), 5)) / 4, 0.0));
		//Colore 
		fig->colors.push_back(vec4(1.0, 0.0, 0.0, 0.0));
	}
	fig->nv = fig->vertici.size();
}

void INIT_SHADER(void) {
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_Update.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {
	Farf.nTriangles = 180;
	costruisci_farfalla(0.0, 0.0, 1.0, 1.0, &Farf);
	crea_VAO_Vector(&Farf);
	Scena.push_back(Farf);

	Cannone.nTriangles = 180;
	costruisci_cannone(0.0, 0.0, 1.0, 1.0, &Cannone);
	crea_VAO_Vector(&Cannone);
	Scena.push_back(Cannone);

	Proiettile.nTriangles = 180;
	costruisci_proiettile(0.0, 0.0, 1.0, 1.0, &Proiettile);
	crea_VAO_Vector(&Proiettile);
	Scena.push_back(Proiettile);

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
	locTime = glGetUniformLocation(programId, "time");
	locRes = glGetUniformLocation(programId, "resolution");

	// Attuare il vero e proprio viewport
	glViewport(0, 0, width, height);
}
void drawScene(void) {

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	vec2 resolution = vec2((float)width, (float)height);

	glUniform1f(locTime, time);
	glUniform2f(locRes, resolution.x, resolution.y);

	/*Matrice di modellazione del cannone */
	Scena[1].Model = mat4(1.0);
	Scena[1].Model = translate(Scena[1].Model, vec3(600.0 + dx, 200.0 + dy, 0.0));
	Scena[1].Model = rotate(Scena[1].Model, angolo, vec3(0.0f, 0.0f, 1.0f));

	/*Matrice di modellazione del proiettile*/
	Scena[2].Model = translate(Scena[1].Model, vec3(posx_Proiettile, 80 + posy_Proiettile, 0.0));
	Scena[2].Model = scale(Scena[2].Model, vec3(30.5, 30.5, 1.0));

	/*Update Scala Matrice di modellazione del cannone */
	Scena[1].Model = scale(Scena[1].Model, vec3(120.0, 120.5, 1.0));

	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));

	//Disegno Proettile
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[2].Model));
	glBindVertexArray(Scena[2].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[2].nVertici);
	glBindVertexArray(0);

	//Disegno Cannone
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[1].Model));
	glBindVertexArray(Scena[1].VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[1].nVertici - 12);
	glDrawArrays(GL_TRIANGLES, Scena[1].nVertici - 12, 12);
	glBindVertexArray(0);

	glutSwapBuffers();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case ' ':
		updateProiettile(0);
		break;

	case 'a':
		dx -= 1;
		break;
	case 'd':
		dx += 1;
		break;

	case 'R':
		angolo += 0.1;
		break;
	case 'r':
		angolo -= 0.1;
		break;

	case 't':
		angolo += 0.1;
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void update(int value) {
	glutTimerFunc(20, update, 0);
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Paolo Cannone con texture diverse");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(myKeyboard);
	glutTimerFunc(20, update, 0);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}
