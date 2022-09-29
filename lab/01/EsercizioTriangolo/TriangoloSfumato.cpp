#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId;
/*
	Le variabili globali di VAO e VBO.
*/
unsigned int VAO;
unsigned int VBO;
unsigned int VBO_COLORI;

float vertices[] = {
	-0.5f, -0.5f, 0.0f, // vertice in basso a sinistra
	0.5f, -0.5f, 0.0f,  // vertice in basso a destra
	0.0f,  0.5f, 0.0f	// vertice in alto 
};

/*
	Creo i colori per il secondo VBO.
*/
float colori[] = {
	1.0f, 0.0f, 0.0f, 1.0f,		// vertice in basso a sinistra
	0.0f, 1.0f, 0.0f, 1.0f,		// vertice in basso a destra
	0.0f, 0.0f, 1.0f, 1.0f		// vertice in alto 
};

void INIT_VAO(void);
void drawScene(void);

/*
	Inizializzazione contesto di esecuzione.
*/
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(400, 150);
	glutCreateWindow("Triangolo OpenGL");
	glutDisplayFunc(drawScene);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_VAO();
	glutMainLoop();
}

void INIT_VAO(void) {
	GLenum ErrorCheckValue = glGetError();
	char* vertexShader = (char*)"vertexShaderSfumato.glsl";
	char* fragmentShader = (char*)"fragmentShaderSfumato.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*
		Per la sfumatura genero un nuovo VBO.
	*/
	glGenBuffers(1, &VBO_COLORI);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_COLORI);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colori), colori, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScene(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// Serve per tante geometrie diverse
	// glBindVertexArray(VAO);

	/*
		Swap dei buffer per il rendering.
	*/
	glutSwapBuffers();
}


