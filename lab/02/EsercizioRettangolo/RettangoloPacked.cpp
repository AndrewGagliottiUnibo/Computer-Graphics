#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId;
unsigned int VAO;
unsigned int VBO, EBO;

/*
	Vertici dei due triangoli che formano il rettangolo nel sistema di coordinate normalizzate.
	I vertici e i colori si possono compattare in un unica definizione stando attenti poi a come si leggono.

	Ancora, ci sono dei vertici ripetuti che possono essere eliminati poichè ridondanti e pesanti da gestire
	per la GPU nei casi di migliaia di vertici da gestire allo stesso tempo.

	Di conseguenza anche gli shader devono essere riscritti.
*/
float vertices[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.0f, -0.3f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f };

/*
	Creo un array di indici che rappresentano gli indici dei rettangoli da gestire.
*/
unsigned int indices[]{
	0, 1, 3,	// Triangolo I
	1, 2, 3		// Triangolo II
};

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
	glutCreateWindow("Quadrato OpenGL Ottimizzato");
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
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*
		Cambio il modo di caricare i vertici.
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/*
		Serve un nuovo buffer per spiegare agli shader come unire i vertici per ottenere la figura desiderata
		priva di ripetizioni in termini di elaborazione. Nell'abilitarla bisogna indicarne il tipo.
	*/
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScene(void)
{
	glBindVertexArray(VAO);
	/*
		Non è più una drawArrays. Gli argomenti sono:
		- modalità.
		- quanti vertici.
		- il tipo.
		- inizio.
	*/
	glDrawElements(GL_TRIANGLE_STRIP, 6, GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
}
