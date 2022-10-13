#include <iostream>
#include "ShaderMaker.h"
#include "lib.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
static unsigned int programId;

#define  PI   3.14159265358979323846


/*
	Possiede tutti i campi per identificare una figura:
	- VAO
	- VBO normale e color
	- triangoli
	- vector: estensione degli array di c che non richiedono una dimensione iniziale specificata.
		contengono oggetti omogenei. I vertici sono sempre 3, i colori 4. Ho coì una lista di
		vertici e colori gestibili in modo semplice.
		GLM come libreria offre come strutture di dati il vec3 e vec4.
	- numero dei vertici.
	- Model: si vogliono rappresentare oggetti in un proprio sistema di riferimento e questi qui,
		tramite matrice, devono essere poi gestiti in coordinate del mondo.
*/
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
	La scena è un vettore di elementi di tipo Figura
*/
vector<Figure> Scena;

Figure Farf = {};
Figure Cuore = {};

void crea_VAO_Vector(Figure* fig) {

	/*
		Anzichè generare VAO singoli staticamente, ho il VAO della figura a cui punta l'input.
		Ogni VAO è legato all'oggetto che definisce.
	*/
	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);

	/*
		Genero, rendo attivo, riempio il VBO della geometria dei vertici.
		size() è un metodo che ritorna il numero delle componenti del chiamante.
		data() è un metodo che ritorna l'indirizzo di inizio della zona di memoria.
	*/
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertices.size() * sizeof(vec3), fig->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	/*
		Genero, rendo attivo, riempio il VBO dei colori.
	*/
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);

	/*
		Adesso carico il VBO dei colori nel layer 2.
	*/
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);
}

void costruisci_cuore(float cx, float cy, float raggiox, float raggioy, Figure* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;

	/*
		Per andare a riempire il campo di vertici vado nel campo e con push_back() riempo la struttura.
		Il metodo richiede come input un tipo che sia compatibile con la struttura da riempire.
	*/
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

	char* vertexShader = (char*)"vertexShader_S.glsl";
	char* fragmentShader = (char*)"fragmentShader_S.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {

	/*
		Attenzione ad usare . oppure -> perchè il primo si usa per i campi e il secondo con i puntatori.
	*/
	Farf.nTriangles = 180;
	costruisci_farfalla(-0.2, -0.2, 0.5, 0.5, &Farf);
	crea_VAO_Vector(&Farf);
	Scena.push_back(Farf);

	Cuore.nTriangles = 180;
	costruisci_cuore(0.5, 0.0, 0.2, 0.2, &Cuore);
	crea_VAO_Vector(&Cuore);

	Scena.push_back(Cuore);
}

void drawScene(void) {
	int k;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	/*
		Riempo la scena con le varie figure: riempo il VAO e lo accendo per poi disegnare i triangoli.
	*/
	for (k = 0; k < Scena.size(); k++) {
		glBindVertexArray(Scena[k].VAO);
		
		/*
			Si specifica la modalità di rendering. I triangle_fan solitamente riempono secondo il fragment shader
			mentre i gl_line non riempono i colori interni ai vertici.
		*/
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv);
	}

	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Polygon OpenGL");
	glutDisplayFunc(drawScene);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
}


