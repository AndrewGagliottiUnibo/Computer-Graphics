#include <iostream>

#include "ShaderMaker.h"
#include "Lib.h"
#include "geometria.h"
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
bool visualizzaAncora =FALSE;
int width = 1024;
int height = 800;
 
int cont_cubi = 0, cont_pir = 0;
int w_up = width;
int h_up = height;
mat4 Projection_text;
string stringa_asse;
string Operazione;
float cameraSpeed = 0.1;
static unsigned int programId, programId_text, MatrixProj, MatrixProj_txt, MatModel, MatView;
unsigned int VAO_Text, VBO_Text;
float raggio_sfera=2.5;
vec3 asse = vec3(0.0, 1.0, 0.0);
int selected_obj = 0;
Mesh Cubo, Piano, Piramide;

mat4 Projection, Model, View;
enum {
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;
enum {
	X,
	Y,
	Z
} WorkingAxis;


void INIT_SHADER(void)

{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_C.glsl";
	char* fragmentShader = (char*)"fragmentShader_C.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);

}



void crea_VAO_Vector(Mesh* mesh)
{

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	//EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);


}


void INIT_VAO(void)
{
	//CUBO
	crea_cubo(&Cubo);
	crea_VAO_Vector(&Cubo);
	Cubo.nome = "Cubo";
	
	//definizione della matrice di Modellazione
	Cubo.Model = mat4(1.0);
	Cubo.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
	Cubo.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
	Cubo.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
	Scena.push_back(Cubo);

	//Piano Suddiviso
 
	crea_piano(&Piano);
	crea_VAO_Vector(&Piano);
	Piano.nome = "Piano";
		//definizione della matrice di Modellazione
	Piano.Model = mat4(1.0);
	Piano.Model = scale(Piano.Model, vec3(20.0f, 1.0f, 20.0f));
	Scena.push_back(Piano);
	
	//Piramide
	crea_piramide(&Piramide);
	crea_VAO_Vector(&Piramide);
	Piramide.nome = "Piramide";
	//definizione della matrice di Modellazione
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
	//ricordare che mat4(1) costruisce una matrice identit� di ordine 4
	 mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	 mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	 mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	 //Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotarion*traslation

		Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;

		glutPostRedisplay();
	
}

void moveCameraForward()
{
	
	//vec4 direction = To do //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//ViewSetup.position += direction * cameraSpeed;
}

void moveCameraBack()
{

	//vec4 direction = To do //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//ViewSetup.position -= direction * cameraSpeed;
}

void moveCameraLeft()
{
	//vec3 direction = To do ; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//vec3 slide_vector = //Direzione ortogonale al piano indiviuato dalla direzione di vista e dall'alto della telecamera
	//ViewSetup.position -= glm::vec4(slide_vector, .0);
	//ViewSetup.target -= glm::vec4(slide_vector, .0);
}

void moveCameraRight()
{

	//vec3 direction = To do ; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//vec3 slide_vector = //Direzione ortogonale al piano indiviuato dalla direzione di vista e dall'alto della telecamera
	//ViewSetup.position += glm::vec4(slide_vector, .0);
	//ViewSetup.target += glm::vec4(slide_vector, .0);
}

void moveCameraUp()
{

	//vec3 direction = To do ; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//vec3 slide_vector = //Direzione ortogonale al piano indiviuato dalla direzione di vista e dall'alto della telecamera
	//vec3 upDirection = //Direzione ortogonale al piano indiviuato da direction e slide-vector 
	//ViewSetup.position -= glm::vec4(upDirection, 0.0);
	//ViewSetup.target -= glm::vec4(upDirection, 0.0);
}

void moveCameraDown()
{
	 
	//vec3 direction = To do ; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	//vec3 slide_vector = //Direzione ortogonale al piano indiviuato dalla direzione di vista e dall'alto della telecamera
	//vec3 upDirection = //Direzione ortogonale al piano indiviuato da direction e slide-vector
	//ViewSetup.position += glm::vec4(upDirection, 0.0);
	//ViewSetup.target += glm::vec4(upDirection, 0.0);
}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	char* intStr;
	string str;
	switch (key)
	{

	case 'a':
		moveCameraLeft(); break;

	case 'd':
		moveCameraRight();
		break;


	case 'w':
		moveCameraForward();
		break;

	case 's':
		moveCameraBack();
		break;

	case 'U':
		moveCameraUp();
		break;

	case 'D':
		moveCameraDown();
		break;

	case 'v':
		visualizzaAncora = TRUE;
	case 'C':  //Si inserisce un cubo
		crea_cubo(&Cubo);
		crea_VAO_Vector(&Cubo);
		cont_cubi += 1;
		 str= std::to_string(cont_cubi);
		 
		Cubo.nome = "Cubo"+str;
		Cubo.Model = mat4(1.0);
		Cubo.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
		Cubo.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
		Cubo.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
		Scena.push_back(Cubo);
		break;
	case 'P':  //Si inserisce una piramide
		crea_piramide(&Piramide);
		crea_VAO_Vector(&Piramide);
		cont_pir += 1;
		str = std::to_string(cont_pir);

		Piramide.nome = "Piramide" + str;
		Piramide.Model = mat4(1.0);
		Piramide.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
		Piramide.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
		Piramide.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
		Scena.push_back(Piramide);
		break;
case 'g':  //Si entra in modalit� di operazione traslazione
	OperationMode = TRASLATING;
	Operazione = "TRASLAZIONE";
	break;
case 'r': //Si entra in modalit� di operazione rotazione
	OperationMode = ROTATING;
	Operazione = "ROTAZIONE";
	break;
case 'S':
	OperationMode = SCALING; //Si entra in modalit� di operazione scalatura
	Operazione = "SCALATURA";
	break;
case 27:
	glutLeaveMainLoop();
	break;
	// Selezione dell'asse
case 'x':
	WorkingAxis = X;  //Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
	stringa_asse = " Asse X";
	break;
case 'y':
	WorkingAxis = Y;  //Seleziona l'asse Y come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
	stringa_asse = " Asse Y";
	break;
case 'z':
	WorkingAxis = Z;
	stringa_asse = " Asse Z";  //Seleziona l'asse Z come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
	break;

default:
	break;
	}

	// Selezione dell'asse per le trasformazioni
	switch (WorkingAxis) {
	case X:	asse = glm::vec3(1.0, 0.0, 0.0);

		break;
	case Y: asse = glm::vec3(0.0, 1.0, 0.0);

		break;
	case Z: asse = glm::vec3(0.0, 0.0, 1.0);

		break;
	default:
		break;
	}

 

	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni
	
	float amount = .01;
	if (key == '+')
		amount *= 1;

			if (key == '-')
				amount *= -1;


	switch (OperationMode) {

//la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor) 
// definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

	case TRASLATING:
		// si passa angle 0 e scale factor =1, 
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
	
void keyboardReleasedEvent(unsigned char key, int x, int y)
{
	{
		switch (key)
		{
		case 'v':
			visualizzaAncora = FALSE;
			break;
		default:
			break;
		}
	}
	glutPostRedisplay();
}
	
void INIT_CAMERA_PROJECTION(void) {
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 20.0, 0.0);
	ViewSetup.target = glm::vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = glm::vec4(0.0, 1.0, 0.0, 0.0);


	//Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

vec3 get_ray_from_mouse(float mouse_x, float mouse_y) {

	//Aggiustameno per via della logica del viewport
	mouse_y=height-mouse_y;

	//mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]  
	//Mappatura di un punto (mouse_x,mouse_y)  mouse_x in [A,B], mouse_y in [C,D]; nel nostro caso l=-1, r=1;
	float x = (2.0f * mouse_x) / width - 1.0f;
	float y = (2.0f * mouse_y) / height - 1.0f;
	vec3 ray_ndn = vec3(x, y, 1);
	
	// Nel clip space hanno coordinate
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// Nell' eye space avranno coordinate
	//Premoltiplicare per la matrice inversa della Projection per passare dalle coordinate di clip 
	//alle coordinate della telecamera
	//funzione di libreria glm
	vec4 ray_eye = inverse(Projection) * ray_clip;
	 
	//Direzione del raggio
	//Ricordare che per individuare una direzione la 4 coordinata deve essere 0
	//essendo una direzione la quarta coordinata è per forza nulla
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// world space
	//Premoltiplicare per la matrice inversa della matrice di Vista per passare dalle coordinate della telecamera
	//alle coordinate del mondo
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	//Normalizzazione della direzione
	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione pi� vicina.
*/
bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - cc;
	
	if (delta < 0.0f) {
		return false;
	} 
	
	if (delta > 0.0f) {
		// quindi se ci sono due intersezioni reali
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		if (t_a < 0.0f) {
			if (t_b < 0.0f) {
				return false;
			}
		}

		return true;
	}
	else {
		float t = -b + sqrt(delta);

		if (t < 0) {
			return false;
		}
		
		*intersection_distance = t;

		return true;
	}

	return false;
}

void mouse(int button, int state, int x, int y)
{
	float xmouse = x;
	float ymouse = y;
	//Costruisce la direzione del raggio in cui punta il mouse chiamando la funzione get_ray_from_mouse(xmouse, ymouse);
	vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

	//facciamo l'intersezione del raggio che ha origine dalla telecamera
	selected_obj = 0;
	float closest_intersection = 10000000.0f;
	for (int i = 0; i < Scena.size(); i++)
	{
		float t_dist = 0.0f;
		//Interseco il raggio che esce dalla camera nella direzione del mouse con la sfera centrata nell'ancora di tutti gli oggetti 
		//posizionati nel mondo per individuare se c'� intersezione con l'oggetto. Utilizzo la funzione ray_sphere
		if (ray_sphere(ViewSetup.position, ray_wor, Scena[i].ancora_world, raggio_sfera, &t_dist)) {
			if (selected_obj == -1 || t_dist < closest_intersection) {
				selected_obj = i;
				closest_intersection = t_dist;
			}
		}
	}
	//cout << "fin" << endl;

	if(selected_obj>-1)
		printf("Oggetto selezionato %s \n", Scena[selected_obj].nome.c_str());

	glutPostRedisplay();
}

void resize(int w, int h)
{
	//Imposto la matrice di Proiezione per il rendering del testo

	Projection_text = ortho(0.0f, (float)width, 0.0f, (float)height);

	//Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ci� che � nel mondo
	 //Se l'aspect ratio del mondo � diversa da quella della finestra devo mappare in modo diverso 
	 //per evitare distorsioni del disegno
	if (AspectRatio_mondo > w / h)   //Se ridimensioniamo la larghezza della Viewport
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_up = (float)w;
		h_up = w / AspectRatio_mondo;
	}
	else {  //Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo 
			//e la finestra sullo schermo sono uguali
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_up = h * AspectRatio_mondo;
		h_up = (float)h;
	}
}




void drawScene(void)
{


	//Utilizzo il program shader per il disegno

	glUseProgram(programId);

	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));


	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Telecamera la cui posizione ruota descrivendo una circonferenza, rimane fisso il punto in cui guarda
	 
	 
	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));

	//Passo al Vertex Shader il puntatore alla matrice View, che sar� associata alla variabile Uniform mat4 Projection
   //all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
	for (int k = 0; k < Scena.size(); k++)
	{
		Scena[k].ancora_world = Scena[k].ancora_obj;
		Scena[k].ancora_world = Scena[k].Model * Scena[k].ancora_world;
		//Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sar� associata alla variabile Uniform mat4 Projection
		//all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glBindVertexArray(Scena[k].VAO);
		
	 	if (visualizzaAncora==TRUE)
		{
			int ind = Scena[k].indici.size() - 1;

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, (Scena[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	 
		glBindVertexArray(0);

	}

	//Imposto il renderizzatore del testo
	RenderText(programId_text, Projection_text, Operazione, VAO_Text, VBO_Text, 10.0f, 650.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	RenderText(programId_text, Projection_text, stringa_asse, VAO_Text, VBO_Text, 10.0f, 700.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	RenderText(programId_text, Projection_text, "Oggetto selezionato", VAO_Text, VBO_Text, 10.0f, 600.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	if (selected_obj > -1)
		RenderText(programId_text, Projection_text, Scena[selected_obj].nome.c_str(), VAO_Text, VBO_Text, 10.0f, 550.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	glutSwapBuffers();
}

/*
void my_passive_mouse(int xpos, int ypos)
{
	float xoffset, yoffset; //Offset tra due posizioni successive del mouse

	float alfa = 0.05; //serve ridimensionare l'offset tra due posizioni successive del mosue
	ypos = height - ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//Calcola l'offset tra due posizioni successive del mouse
	 

	//Incrementa gli angoli Theta e Phi mediante gli offset del mosue
	Theta += xoffset;
	Phi += yoffset;

	// Impostare Range dell'angolo Phi

	//Calcoare la direzione facendo uso del punto sulla sfera individuato da Theta e Phi
	//Normalizzare la direzione e usarla per inviduare il target come somma tra posizione e direzione.

	//ViewSetup.target = To do

	glutPostRedisplay();
}

*/
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);


	//Inizializzo finestra per il rendering della scena 3d con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	//glutPassiveMotionFunc(my_passive_mouse);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
 
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

	bool b = true;

	float x = b;
	printf("%d", x);
	
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sar� poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");

	glutMainLoop();
}

