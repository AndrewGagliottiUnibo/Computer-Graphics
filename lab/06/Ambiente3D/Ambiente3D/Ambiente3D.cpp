#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "geometria.h"
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H

int width = 1024;
int height = 1024;

int cont_cubi = 1, cont_pir = 1;
int w_up = width;
int h_up = height;

mat4 Projection_text, Projection, Model, View;
string stringa_asse;
string Operazione;

static unsigned int programId, programId_text, MatrixProj, MatModel, MatView;
unsigned int VAO_Text, VBO_Text;
double delay = 0.0;

vec3 asse = vec3(0.0, 1.0, 0.0);
int selected_obj = 0;
Mesh Cubo, Piano, Piramide;

// Per la selezione della  Trasformazione di Modellazione
enum
{
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;

// Per la selezione dell'asse intorno a cui effettuare la trasformazione di Modellazione selezionata
enum
{
	X,
	Y,
	Z
} WorkingAxis;

void INIT_SHADER(void)

{
	GLenum ErrorCheckValue = glGetError();

	char *vertexShader = (char *)"vertexShader_C.glsl";
	char *fragmentShader = (char *)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	// Generazione del program shader per la gestione del testo
	vertexShader = (char *)"VertexShader_Text.glsl";
	fragmentShader = (char *)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);
}

void crea_VAO_Vector(Mesh *mesh)
{

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	// Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	// Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);

	// EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void INIT_VAO(void)
{
	// CUBO
	crea_cubo(&Cubo);
	crea_VAO_Vector(&Cubo);
	Cubo.nome = "Cubo";

	// definizione della matrice di Modellazione
	Cubo.Model = mat4(1.0);
	Cubo.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
	Cubo.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
	Cubo.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
	Scena.push_back(Cubo);

	// Piano Suddiviso
	crea_piano(&Piano);
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano";
	// definizione della matrice di Modellazione
	Piano.Model = mat4(1.0);
	Piano.Model = scale(Piano.Model, vec3(20.0f, 1.0f, 20.0f));
	Scena.push_back(Piano);

	// Piramide
	crea_piramide(&Piramide);
	crea_VAO_Vector(&Piramide);
	Piramide.nome = "Piramide";
	// definizione della matrice di Modellazione
	Piramide.Model = mat4(1.0);
	Piramide.Model = translate(Piramide.Model, vec3(-1.5, 0.0, 0.5));
	Piramide.Model = scale(Piramide.Model, vec3(2.0f, 2.0f, 2.0f));
	Piramide.Model = translate(Piramide.Model, vec3(-0.5, 0.0, -0.5));
	Scena.push_back(Piramide);
}
void INIT_VAO_Text(void)
{

	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO_Text);
	glGenBuffers(1, &VBO_Text);
	glBindVertexArray(VAO_Text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
{
	// to do:
	// Costruire la matrice di traslazione,scala e rotazione con i parametri in ingresso
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	// Modificare la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotation*traslation
	Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;

	glutPostRedisplay();
}
void keyboardPressedEvent(unsigned char key, int x, int y)
{
	char *intStr;
	unsigned char newKey = toupper(key);
	string str;

	switch(key) 
	//switch (newKey)
	{
	case 'C':
		// Si inserisce un cubo
		// Chiamare la funzione che crea il cubo
		//  Creare il VAO del Cubo
		crea_cubo(&Cubo);
		crea_VAO_Vector(&Cubo);
		cont_cubi += 1;
		str = std::to_string(cont_cubi);
		Cubo.nome = "Cubo " + str;

		// Definire la matrice di modellazione iniziale del cubo
		// Fare il pushback del cubo nel vector Scena
		Cubo.Model = mat4(1.0);
		Cubo.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
		Cubo.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
		Cubo.Model = translate(Cubo.Model, vec3(-0.2 + delay, 0.8 + delay, -0.2 + delay));
		Scena.push_back(Cubo);
		delay += 0.1;
		break;

	case 'P': // Si inserisce una piramide
		// Chiamare la funzione che crea la piramide
		//  Creare il VAO del della piramide
		crea_piramide(&Piramide);
		crea_VAO_Vector(&Piramide);
		cont_pir += 1;
		str = std::to_string(cont_pir);
		Piramide.nome = "Piramide " + str;

		// Definire la matrice di modellazione iniziale della piramide
		// Fare il pushback del cubo nel vector Scena
		Piramide.Model = mat4(1.0);
		Piramide.Model = translate(Piramide.Model, vec3(3.5, 0.5, 2.5));
		Piramide.Model = scale(Piramide.Model, vec3(2.0f, 2.0f, 2.0f));
		Piramide.Model = translate(Piramide.Model, vec3(-0.8 + delay, 0.8 + delay, -0.2 + delay));
		Scena.push_back(Piramide);
		delay += 0.1;
		break;

	case 'G': // Si entra in modalità di operazione traslazione
		OperationMode = TRASLATING;
		Operazione = "TRASLAZIONE"; // Stringa da visualizzare sulla finestra
		break;
	case 'R': // Si entra in modalità di operazione rotazione
		OperationMode = ROTATING;
		Operazione = "ROTAZIONE"; // Stringa da visualizzare sulla finestra
		break;
	case 'S':
		OperationMode = SCALING;  // Si entra in modalità di operazione scalatura
		Operazione = "SCALATURA"; // Stringa da visualizzare sulla finestra
		break;
	case 27:
		glutLeaveMainLoop();
		break;
	// Selezione dell'asse
	case 'X':
		WorkingAxis = X; // Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse X";
		break;
	case 'Y':
		WorkingAxis = Y; // Seleziona l'asse Y come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse Y";
		break;
	case 'Z':
		WorkingAxis = Z;
		stringa_asse = " Asse Z"; // Seleziona l'asse Z come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		break;

	default:
		break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis)
	{
	case X:
		asse = glm::vec3(1.0, 0.0, 0.0);

		break;
	case Y:
		asse = glm::vec3(0.0, 1.0, 0.0);

		break;
	case Z:
		asse = glm::vec3(0.0, 0.0, 1.0);

		break;
	default:
		break;
	}

	glutPostRedisplay();

	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

	float amount = .01;
	if (key == '+')
		amount *= 1;

	if (key == '-')
		amount *= -1;

	switch (OperationMode)
	{

	/*
	la funzione
	modifyModelMatrix( vec3 translation_vector,  vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
	definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato,
	per poterlo traslare, ruotare scalare.*/
	case TRASLATING:
		// si passa angle 0 e scale factor =1,
		// si moltiplica perché è un vec3, agisci solo su quella riga.
		modifyModelMatrix(asse * amount, asse, 0.0f, 1.0f);
		break;
	case ROTATING:
		// SI mette a zero il vettore di traslazione (vec3(0) e ad 1 il fattore di scale
		modifyModelMatrix(glm::vec3(0), asse, amount * 2.0f, 1.0f);
		break;
	case SCALING:
		// SI mette a zero il vettore di traslazione (vec3(0), angolo di rotazione a 0 e ad 1 il fattore di scala 1+amount.
		modifyModelMatrix(glm::vec3(0), asse, 0.0f, 1.0f + amount);
		break;
	}
	glutPostRedisplay();
}

void INIT_CAMERA_PROJECTION(void)
{
	// Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 20.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);

	// Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

void mouseWheel(int button, int dir, int x, int y)
{
	// Scorrendo la rotellina del mouse si determina l'indice nella struttura scena dell'oggetto  che so vuole selezionare
	// selected_obj -= dir;
	// To do: se selected_obj <0 settare selected_obj=0;
	//  se selected_obj > Scena.size() - 1  settare selected_obj=> Scena.size() - 1;
	selected_obj -= dir;
	if (selected_obj > Scena.size() - 1)
		selected_obj = selected_obj - 1;

	if (selected_obj < 0)
		selected_obj = 0;

	printf("Selected object %d \n", selected_obj);

	glutPostRedisplay();

	return;
}

void resize(int w, int h)
{
	// Imposto la matrice di Proiezione per il rendering del testo

	Projection_text = ortho(0.0f, (float)width, 0.0f, (float)height);

	// Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); // Rapporto larghezza altezza di tutto ciò che è nel mondo
																// Se l'aspect ratio del mondo è diversa da quella della finestra devo mappare in modo diverso
																// per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h) // Se ridimensioniamo la larghezza della Viewport
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_up = (float)w;
		h_up = w / AspectRatio_mondo;
	}
	else
	{ // Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo
	  // e la finestra sullo schermo sono uguali
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_up = h * AspectRatio_mondo;
		h_up = (float)h;
	}
}

void drawScene(void)
{

	// Utilizzo il program shader per il disegno

	glUseProgram(programId);

	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));

	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));

	// Passo al Vertex Shader il puntatore alla matrice View, che sarà associata alla variabile Uniform mat4 Projection
	// all'interno del Vertex shader. Uso l'identificatio MatView

	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
	for (int k = 0; k < Scena.size(); k++)
	{
		// Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sarà associata alla variabile Uniform mat4 Projection
		// all'interno del Vertex shader. Uso l'identificatio MatModel
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glBindVertexArray(Scena[k].VAO);

		glDrawElements(GL_TRIANGLES, Scena[k].indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	// Imposto il renderizzatore del testo

	RenderText(programId_text, Projection_text, Operazione, VAO_Text, VBO_Text, 10.0f, 650.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, stringa_asse, VAO_Text, VBO_Text, 10.0f, 700.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, "Oggetto selezionato", VAO_Text, VBO_Text, 10.0f, 600.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	if (selected_obj > -1)
		RenderText(programId_text, Projection_text, Scena[selected_obj].nome.c_str(), VAO_Text, VBO_Text, 10.0f, 550.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutMouseWheelFunc(mouseWheel);
	glutKeyboardFunc(keyboardPressedEvent);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	INIT_CAMERA_PROJECTION();
	INIT_VAO_Text();
	Init_Freetype();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	// QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	// QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	// QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");

	glutMainLoop();
}
