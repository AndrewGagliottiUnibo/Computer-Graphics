#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId;

unsigned int VAO;
unsigned int VBO, VBO_COLORI;

/*
	Cambio il canale alfa per gestire la visibilita: non basrta ciò, bisogna interagire con gli shader poiché
	la colorazione dei pixel avviene alla fine ..  vado nel main.
*/
float vertices[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,

	// Sposto le coordinate del secondo triangolo
	0.0f, -0.5f, 0.0f, 0.5f, 1.0f, 0.3f, 0.6f,
	-0.8f, 0.8f, 0.0f, 1.0f, 0.3f, 1.0f, 0.6f,
	-0.8f, -0.8f, 0.0f, 1.0f, 0.3f, 0.0f, 0.6f };

void INIT_SHADER(void);
void INIT_VAO(void);
void drawScene(void);

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Triangoli blended");
	glutDisplayFunc(drawScene);

	/*
		Gestisco la trasparenza.
		Modifico il funzionamento del canale alpha affinché si colori diversamente quella parte di schermo.
		I colori si mescolano!
	*/
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glutSwapBuffers();
}
