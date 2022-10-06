#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

static unsigned int programId;

unsigned int VAO;
unsigned int VBO, VBO_COLORI;

// Vertici dei due triangoli che formano il rettangolo nel sistema di coordinate normalizzate.
float vertices[] = {
	/*
		posizioni che si leggono in senso antiorario a partire da in basso a sinistra.
		L'interpolazione dei colori da rosso a bianco passa attraverso delle sfumature di rosa.
	*/
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	0.5f, 0.5f, 0.0f,
	-0.5f, 0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f};

float colori[] = {
	/*
		I colori sono modificabili applicando un ragionamento diverso sulla interpolazione.
	*/	
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f};

void INIT_SHADER(void);
void INIT_VAO(void);
void drawScene(void);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Quadrato OpenGL Classico");
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
		Carico i dati vertices sulla GPU e spiego agli shader come leggere i dati.
		Gestione statica perchè la figura sulla GPU è sempre lei e non cambia.
		Alla fine attivo l'array dei vertici di posizione 0.
	*/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &VBO_COLORI);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_COLORI);

	glBufferData(GL_ARRAY_BUFFER, sizeof(colori), colori, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	/*
		Scollego il VAO - good practice - e pulisco il buffer dei colori per evitare errori di rappresentazione.
		OpenGL ragiona come una macchina a stati e quindi le chiamate di attivazione iniziano e finiscono a turni
		o stati indipendenti.
	*/
	glBindVertexArray(0);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}
