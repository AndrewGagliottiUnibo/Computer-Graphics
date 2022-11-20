#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

const float PI = 3.1415926535897932384626433832;

static unsigned int programId;
unsigned int VAO;
unsigned int VBO;

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
void buildButterfly(float cx, float cy, Points* butterfly);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Farfalla");
	glutDisplayFunc(drawScene);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
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
	buildButterfly(0.0, 0.0, points);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Carico il buffer diversamente.
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
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, nVertices);
	glutSwapBuffers();
}

void buildButterfly(float cx, float cy, Points* butterfly) {

	// Alcuni contatori:
	int components = 0;
	int i = 0;
	float t;

	// Definisco uno step che ci permette di dividere il cerchio in parti uguali.
	float step = (2 * PI) / nPoints;

	// Definisco vertici e colori del triangolo.
	// I colori sfumano verso il bianco
	butterfly[components].x = cx;
	butterfly[components].y = cy;
	butterfly[components].z = 0.0;

	butterfly[components].r = 0.7;
	butterfly[components].g = 0.8;
	butterfly[components].b = 0.3;
	butterfly[components].a = 1.0;

	for (i = 0; i <= nPoints; i++) {
		t = (double)i * step;
		components++;

		butterfly[components].x = (sin(t) * (exp(cos(t)) - (2 * (double) cos(4 * t))) + pow(sin(t / 12), 5)) / 4;
		butterfly[components].y = (cos(t) * (exp(cos(t)) - (2 * (double) cos(4 * t))) + pow(sin(t / 12), 5)) / 4;
		butterfly[components].z = 0.0;

		butterfly[components].r = 0.8;
		butterfly[components].g = 0.5;
		butterfly[components].b = 1.0;
		butterfly[components].a = 1.0;
	}
}
