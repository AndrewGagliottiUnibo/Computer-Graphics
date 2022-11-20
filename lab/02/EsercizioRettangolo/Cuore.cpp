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

int nPoints = 10;
int nVertices = nPoints + 2;
Points* points = new Points[nVertices];

void INIT_SHADER(void);
void INIT_VAO(void);
void drawScene(void);
void buildHeart(float cx, float cy, Points* heart);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cuore");
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
	buildHeart(0.0, 0.0, points);
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

void buildHeart(float cx, float cy, Points* heart) {

	// Alcuni contatori:
	int components = 0;
	int i = 0;
	float t;

	// Definisco uno step che ci permette di dividere il cerchio in parti uguali.
	float step = (2 * PI) / nPoints;

	// Definisco vertici e colori del triangolo.
	// I colori sfumano verso il bianco
	heart[components].x = cx;
	heart[components].y = cy;
	heart[components].z = 0.0;

	heart[components].r = 0.7;
	heart[components].g = 0.8;
	heart[components].b = 0.3;
	heart[components].a = 1.0;

	for (i = 0; i <= nPoints; i++) {
		t = (double)i * step;
		components++;

		heart[components].x = pow(sin(t), 3);
		heart[components].y = (13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t)) / 16;
		heart[components].z = 0.0;

		heart[components].r = 0.8;
		heart[components].g = 0.5;
		heart[components].b = 1.0;
		heart[components].a = 1.0;
	}
}
