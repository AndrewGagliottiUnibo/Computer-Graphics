#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"
#include <GL/glew.h>
#include <GL/freeglut.h>

#define  PI   3.14159265358979323846

static unsigned int programId;
static unsigned int ProjectionMatrix;
static unsigned int ModelMatrix;
static unsigned int width = 800;
static unsigned int height = 800;

float angolo = 0.0f;
float scaleSize = 1.0f;
float dx = 0;
float dy = 0;

mat4 Projection;

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;

	vector<vec3> vertices;
	vector<vec4> colors;

	int nv;

	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
} Figure;

/*
	Ogni figura ha una matrice mat4 che serve per modellarla.
*/
vector<Figure> Scena;

Figure Farf = {};
Figure Cuore = {};

void crea_VAO_Vector(Figure* fig) {

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);

	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertices.size() * sizeof(vec3), fig->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void costruisci_cuore(float cx, float cy, float raggiox, float raggioy, Figure* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;

	fig->vertices.push_back(vec3(cx, cy, 0.0));
	fig->colors.push_back(vec4(255.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA;
		fig->vertices.push_back(vec3(cx + raggiox * (16 * pow(sin(t), 3)) / 16, cy + raggioy * ((13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t)) / 16), 0.0));
		fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));
	}
	fig->nv = fig->vertices.size();
}

void costruisci_farfalla(float cx, float cy, float raggiox, float raggioy, Figure* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;

	fig->vertices.push_back(vec3(cx, cy, 0.0));
	fig->colors.push_back(vec4(150.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++) {
		t = (float)i * stepA;
		fig->vertices.push_back(vec3(cx + raggiox * (sin(t) * (exp(cos(t)) - 2 * cos(4 * t)) + pow(sin(t / 12), 5)) / 4, cy + raggioy * (cos(t) * (exp(cos(t)) - 2 * cos(4 * t)) + pow(sin(t / 12), 5)) / 4, 0.0));
		fig->colors.push_back(vec4(1.0, 0.0, 0.0, 0.0));


	}
	fig->nv = fig->vertices.size();
}

void INIT_SHADER(void) {
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {

	/*
		Impostiamo a valori base per poi creare la matrice di modellazione.
	*/
	Farf.nTriangles = 180;
	costruisci_farfalla(0.0, 0.0, 1.0, 1.0, & Farf);
	crea_VAO_Vector(&Farf);
	Scena.push_back(Farf);

	Cuore.nTriangles = 180;
	costruisci_cuore(0.0, 0.0, 1.0, 1.0, &Cuore);
	crea_VAO_Vector(&Cuore);

	Scena.push_back(Cuore);

	/*
		Siccome le location di proiezione non cambiano nel tempo le posso definire qui.
		ortho() contiene:
		- min x
		- max x
		- min y
		- max y
	*/
	Projection = ortho(0.0f, float(width), 0.0f, float(height));

	/*
		La matrice di proiezione vuole il programma su cui lavorare, il nome della variabile uniforme del vertex shader
		per come è definita nel programId. Stessa cosa per la matrice di modellazione.
	*/
	ProjectionMatrix = glGetUniformLocation(programId, "Projection");
	ModelMatrix = glGetUniformLocation(programId, "Model");
}

void drawScene(void) {
	int k;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(ProjectionMatrix, 1, GL_FALSE, value_ptr(Projection));

	for (k = 0; k < Scena.size(); k++) {
		
		/*
			Istanzio qui la matrice di modellazione di ogni figura.
			La traslo.
			La scalo.
		*/
		if (k == 0) {
			Scena[k].Model = mat4(1.0f);
			Scena[k].Model = translate(Scena[k].Model, vec3(600.0f + dx, 200.0f + dy, 0.0f));
			Scena[k].Model = scale(Scena[k].Model, vec3(100.0f * scaleSize, 100.0f * scaleSize, 1.0f * scaleSize));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(1.0f, 1.0f, 1.0f));
		}
		else {
			Scena[k].Model = mat4(1.0f);
			Scena[k].Model = translate(Scena[k].Model, vec3(400.0f + dx, 400.0f + dy, 0.0f));
			Scena[k].Model = scale(Scena[k].Model, vec3(100.0f * scaleSize, 100.0f * scaleSize, 1.0f * scaleSize));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0f, 0.0f, 1.0f));
		}

		/*
			Passo ora allo shader le informazioni sulla matrice uniformi.
			- Cosa passi.
			- Quante ne passi.
			- Trasposizione?
			- Puntatore di cosa stai passando.

			Lo shader utilizzerà le matrici come input.
		*/
		glUniformMatrix4fv(ModelMatrix, 1, GL_FALSE, value_ptr(Scena[k].Model));

		glBindVertexArray(Scena[k].VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv);
	}

	glutSwapBuffers();
}

/*
	Incrementa gli angoli di rotazione.
*/
void update(int value) {
	angolo += 1.0f;
	scaleSize *= 1.1f;

	if (scaleSize > 2) {
		scaleSize = 1.0f;
	}
	glutTimerFunc(50, update, 0);
	glutPostRedisplay();
}

/*
	Muove la figura.
*/
void interaction(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a' :
		dx--;
		break;
	case 'd':
		dx++;
		break;
	case 'w':
		dy++;
		break;
	case 's':
		dy--;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Events OpenGL");
	glutDisplayFunc(drawScene);

	/*
		Creiamo ancora un evento di ozio.
	*/
	//glutTimerFunc(250, update, 0);

	/*
		Interazione.
	*/
	glutKeyboardFunc(interaction);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}


