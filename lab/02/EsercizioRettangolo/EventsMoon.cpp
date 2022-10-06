#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

const float PI = 3.14158592573;

static unsigned int programId;
unsigned int VAO;
unsigned int VBO;
float r = 0.3;
float g = 0.1;
float b = 0.4;

/*
	Variabili per il cechio.
*/
typedef struct {
	// Position.
	float x;
	float y;
	float z;

	// Colors.
	float r;
	float g;
	float b;
	float a;
} Points;

int nPoints = 100;
int nVertices = nPoints + 2;
Points* points = new Points[nVertices];

void INIT_SHADER(void);
void INIT_VAO(void);
void drawScene(void);
void buildMoon(float cx, float cy, float radius, Points* moon);
void mouse(int button, int state, int x, int y);
void update(int value);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Luna con eventi");
	glutDisplayFunc(drawScene);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();

	/*
		Gestisco l'oziosità della grafica con un evento idle.
	*/
	glutTimerFunc(145, update, 0);

	glutMainLoop();
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShaderC.glsl";
	char* fragmentShader = (char*)"fragmentShaderC.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void)
{
	buildMoon(0.0, 0.0, 0.3, points);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Points), &points[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScene(void)
{
	/*
		Creo una funzione per la gestione degli eventi subito dopo aver impostato i colori desiderati.
	*/
	glClearColor(r, g, b, 1.0);
	glutMouseFunc(mouse);

	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, nVertices);
	glutSwapBuffers();
}

void buildMoon(float cx, float cy, float radius, Points* moon) {

	int components = 0;
	int i = 0;
	float t;

	// Definisco uno step che ci permette di dividere il cerchio in parti uguali.
	float step = (3 * PI) / nPoints;

	// Definisco vertici e colori del triangolo.
	moon[components].x = cx;
	moon[components].y = cy;
	moon[components].z = 0.0;

	moon[components].r = 0.7;
	moon[components].g = 0.8;
	moon[components].b = 0.3;
	moon[components].a = 1.0;

	for (i = 0; i <= nPoints; i++) {
		t = (double)i * step;
		components++;

		moon[components].x = 3 * radius * sin(t);
		moon[components].y = 0.5 - (radius * ((cos(2 * t) - cos(t))));
		moon[components].z = 0.0;

		moon[components].r = 0.8;
		moon[components].g = 0.5;
		moon[components].b = 1.0;
		moon[components].a = 1.0;
	}
}

void mouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		// Cambio il colore dello sfondo
		if (g > 1.0) {
			g = 0.0;
		}
		g += 0.1;

		// Forza il ridisegno della figura con i parametri aggiornati.
		glutPostRedisplay();
	}
}

void update(int value) {
	g += 0.01;
	if (g > 1) {
		g = 0.0;
	}
	
	b += 0.01;
	if (b > 1) {
		b = 0.0;
	}

	glutTimerFunc(145, update, 0);
	glutPostRedisplay();
}
