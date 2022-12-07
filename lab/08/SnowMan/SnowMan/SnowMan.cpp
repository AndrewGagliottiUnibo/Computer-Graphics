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
 
static int last_mouse_pos_Y;
static int last_mouse_pos_X;
static bool moving_trackball = 0;
int cont_cubi = 0, cont_pir = 0;
int w_up = width;
int h_up = height;
mat4 Projection_text;
string stringa_asse;
string Operazione;
float cameraSpeed = 0.1;
static unsigned int programId, programId_text, programId_Sfondo, MatrixProj, MatrixProj_txt, MatModel, MatView;
static unsigned int MatrixProjS, MatViewS, MatModelS, loc_timeS, loc_res, loc_mouse, loc_sceltaVs, loc_time;

unsigned int VAO_Text, VBO_Text;
float raggio_sfera=2.5;
vec3 asse = vec3(0.0, 1.0, 0.0);
int selected_obj = 0;
Mesh Cubo, Piano, Piramide;
static vector<Mesh> Snowman, Bandiera;
vec2 resolution, mousepos;
float posxN=-5, posyN=0.0;
float angolo = 0.0;
bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;
float Theta = -90.0f;
float Phi = 0.0f;


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

	vertexShader = (char*)"vertexShader_C.glsl";
	fragmentShader = (char*)"fragmentShader_Sfondo.glsl";

	programId_Sfondo = ShaderMaker::createProgram(vertexShader, fragmentShader);


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


void INIT_VAO(void) {
	Mesh Sfondo, Palo,Telo,Casa, cappello, falda, testa, naso, corpo, braccio_s, braccio_d, occhio_s, occhio_d;

	crea_piano(&Sfondo, vec4(0.2, 0.2, 0.9, 1.0));
	crea_VAO_Vector(&Sfondo);
	Sfondo.nome = "Sfondo";
	Sfondo.Model = mat4(1.0);
	Sfondo.Model = translate(Sfondo.Model, vec3(0.0, 0.0, -100.0));
	Sfondo.Model = scale(Sfondo.Model, vec3(5000.0f, 5000.00f, 1.0f));
	Sfondo.Model = rotate(Sfondo.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	Scena.push_back(Sfondo);
	
 	//Piano 
	crea_piano(&Piano,vec4(0.9,0.9,0.9,1.0));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Terreno";
		//definizione della matrice di Modellazione
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -1.5, 0.0));
	Piano.Model = scale(Piano.Model, vec3(5000.0f, 1.0f, 5000.0f));
	Scena.push_back(Piano);

	//casa
	crea_casa(&Casa, vec4(0.0, 1.0, 0.0, 1.0), vec4(1.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Casa);
	Casa.Model = mat4(1.0);
	Casa.Model = translate(Casa.Model, vec3(8.0, 0.0, 0.0));
	Casa.Model = scale(Casa.Model, vec3(2.0, 2.0, 2.0));
	Casa.nome = "Casa";
	centri.push_back(vec3(0.0, 2.75, 0.0));
	raggi.push_back(0.5);
	Scena.push_back(Casa);

	//Snowman
	//COSTRUZIONE DEL PERSONAGGIO: OGNI SUA PARTE VIENE MEMORIZZATA NELLA STRUTTURA Snowman
	//CAPPELLO
	crea_cilindro(&cappello, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&cappello);
	cappello.Model = mat4(1.0);
	cappello.Model = translate(cappello.Model, vec3(0.0, 2.25, 0.0));
	cappello.nome = "Cappello";
	centri.push_back(vec3(0.0, 2.75, 0.0));
	raggi.push_back(0.5);
	Snowman.push_back(cappello);

	//FALDA
	crea_toro(&falda, vec4(0.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&falda);
	falda.Model = mat4(1.0);
	falda.Model = translate(falda.Model, vec3(0.0, 2.25, 0.0));
	falda.Model = scale(falda.Model, vec3(1.0, 0.2, 0.5));
	falda.nome = "falda";
	centri.push_back(vec3(0.0, 2.0, 0.0));
	raggi.push_back(0.5);
	Snowman.push_back(falda);

	//Testa SNOWMAN
	 crea_sfera(&testa,vec4(1.0,1.0,1.0,1.0));
	 crea_VAO_Vector(&testa);
	 testa.Model = mat4(1.0);
	 testa.Model = translate(testa.Model, vec3(0.0, 1.75, 0.0));
	 testa.Model = scale(testa.Model, vec3(0.75, 0.75, 0.75));
	 testa.nome = "Testa";
	 centri.push_back(vec3(0.0, 1.75, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(testa);
	
	 //OCCHIO SINISTRO
	 crea_sfera(&occhio_s, vec4(1.0, 0.0, 0.0, 1.0));
	 crea_VAO_Vector(&occhio_s);
	 occhio_s.Model = mat4(1.0);
	 occhio_s.Model = translate(occhio_s.Model, vec3(-0.4, 1.75, 0.8));
	 occhio_s.Model = scale(occhio_s.Model, vec3(0.15, 0.15, 0.15));
	 occhio_s.nome = "occhio sinistro";
	 centri.push_back(vec3(-0.5, 1.75, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(occhio_s);

	 //OCCHIO DESTRO
	 crea_sfera(&occhio_d, vec4(1.0, 0.0, 0.0, 1.0));
	 crea_VAO_Vector(&occhio_d);
	 occhio_d.Model = mat4(1.0);
	 occhio_d.Model = translate(occhio_d.Model, vec3(0.4, 1.75, 0.8));
	 occhio_d.Model = scale(occhio_d.Model, vec3(0.15, 0.15, 0.15));
	 occhio_d.nome = "Occhio destro";
	 centri.push_back(vec3(0.5, 1.75, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(occhio_d);

	 //NASO
	 crea_sfera(&naso, vec4(1.0, 0.0, 1.0, 1.0));
	 crea_VAO_Vector(&naso);
	 naso.Model = mat4(1.0);
	 naso.Model = translate(naso.Model, vec3(0.0, 1.6, 0.4));
	 naso.Model = scale(naso.Model, vec3(0.2, 0.2, 0.6));
	 centri.push_back(vec3(0.0, 1.6, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(naso);

	  //Corpo
	 crea_sfera(&corpo, vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&corpo);
	 corpo.Model = mat4(1.0);
	 corpo.Model = translate(corpo.Model, vec3(0.0,-0.2, 0.0));
	 corpo.Model = scale(corpo.Model, vec3(1.2, 1.4, 1.0));
	 raggi.push_back(0.5);
	 corpo.nome = "Corpo";
	 centri.push_back(vec3(0.0, 0.0, 0.0));
	 Snowman.push_back(corpo);
	 raggi.push_back(0.5);

	 //Braccio sinistro
	 crea_sfera(&braccio_s, vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&braccio_s);
	 braccio_s.Model = mat4(1.0);
	 braccio_s.Model = translate(braccio_s.Model, vec3(-1.0, 0.0, 0.0));
	 braccio_s.Model = rotate(braccio_s.Model, radians(65.0f), vec3(0.0, 0.0, 1.0));
	 braccio_s.Model = scale(braccio_s.Model, vec3(1, 0.25, 0.25));
	 braccio_s.nome = "braccio sinistro";
	 centri.push_back(vec3(-1.0, 0.0, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(braccio_s);

	 //Braccio Destro
	 crea_sfera(&braccio_d, vec4(1.0, 1.0, 1.0, 1.0));
	 crea_VAO_Vector(&braccio_d);
	 braccio_d.Model = mat4(1.0);
	 braccio_d.Model = translate(braccio_d.Model, vec3(1.0, 0.0, 0.0));
	 braccio_d.Model = rotate(braccio_d.Model, radians(-65.0f), vec3(0.0, 0.0, 1.0));
	 braccio_d.Model = scale(braccio_d.Model, vec3(1, 0.25, 0.25));
	 braccio_d.nome = "braccio destro";
	 centri.push_back(vec3(1.0, 0.0, 0.0));
	 raggi.push_back(0.5);
	 Snowman.push_back(braccio_d);
}


void INIT_VAO_Text(void) {
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
	//ricordare che mat4(1) costruisce una matrice identità di ordine 4
	 mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	 mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	 mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	 //Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotarion*traslation

	Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;

	glutPostRedisplay();
	
}

//Movimento della telecamera
void moveCameraForward()
{

	vec4 direction = ViewSetup.target - ViewSetup.position;    //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	ViewSetup.position += direction * cameraSpeed;
}

void moveCameraBack()
{

	 vec4 direction = ViewSetup.target - ViewSetup.position; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	ViewSetup.position -= direction * cameraSpeed;
}

void moveCameraLeft()
{
	 vec3 direction = ViewSetup.target - ViewSetup.position; //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	 vec3 slide_vector =  cross(direction,  vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position -=  vec4(slide_vector, .0);
	ViewSetup.target -=  vec4(slide_vector, .0);
}

void moveCameraRight()
{

	 vec3 direction = ViewSetup.target - ViewSetup.position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	 vec3 slide_vector =  cross(direction,  vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position +=  vec4(slide_vector, .0);
	ViewSetup.target +=  vec4(slide_vector, .0);
}

void moveCameraUp()
{

	 vec3 direction = ViewSetup.target - ViewSetup.position;  //Direzione lungo cui si sposta la telecamera in coordinate del mondo
	 vec3 slide_vector =  normalize(glm::cross(direction,  vec3(ViewSetup.upVector)));
	 vec3 upDirection =  cross(direction, slide_vector) * cameraSpeed;
	ViewSetup.position -=  vec4(upDirection, 0.0);
	ViewSetup.target -=  vec4(upDirection, 0.0);
}

void moveCameraDown()
{
	 
	 vec4 direction = ViewSetup.target - ViewSetup.position;
	 vec3 slide_vector =  normalize( cross( vec3(direction),  vec3(ViewSetup.upVector)));
	 vec3 upDirection =  cross( vec3(direction), slide_vector) * cameraSpeed;
	  ViewSetup.position +=  vec4(upDirection, 0.0);
	  ViewSetup.target +=  vec4(upDirection, 0.0);
}

//Gestione eventi tastiera per il movimento della telecamera
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

	case 'v':  //Visualizzazione ancora
		visualizzaAncora = TRUE;
		break;
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
case 'g':  //Si entra in modalità di operazione traslazione
	OperationMode = TRASLATING;
	Operazione = "TRASLAZIONE";
	break;
case 'r': //Si entra in modalità di operazione rotazione
	OperationMode = ROTATING;
	Operazione = "ROTAZIONE";
	break;
case 'S':
	OperationMode = SCALING; //Si entra in modalità di operazione scalatura
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
	
void INIT_CAMERA_PROJECTION(void)
{
	//Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 30.0, 0.0);
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

	mouse_y = height - mouse_y;

	// mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]  
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = (2.0f * mouse_y) / height - 1.0;
	float z = 1.0f;
	vec3  ray_nds = vec3(x, y, z);
	// Nel clip space hanno coordinate: nota bene la terza coordinata uguale a -1
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// Le coordinate Nell' eye space si ottengono premoltiplicando per l'inversa della matrice Projection.

	vec4 ray_eye = inverse(Projection) * ray_clip;

	//Direzione del raggio
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// le coordinate nel world space: si ottengono premoltiplicando per l'inversa della matrice di Vista 
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p iù vicina.
*/

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float* intersection_distance) {

	//Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - cc;

	if (delta < 0)  //Il raggio non interseca la sfera
		return false;
	//Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione 
	//più vicina all'osservatore (la t più piccola)
	if (delta > 0.0f) {
		//calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		//Caso di intersezioni dietro l'osservatore
		if (t_a < 0.0) {
			if (t_b < 0)
				return false;
		}

		return true;
	}
	//Caso in cui il raggio è tangente alla sfera: un interesezione con molteplicità doppia.
	if (delta == 0) {
		float t = -b + sqrt(delta);
		if (t < 0)
			return false;
		*intersection_distance = t;
		return true;
	}

	return false;
}

void mouse(int button, int state, int x, int y)
{

	switch (button) {
		//Con il tasto sinistro premuto si attiva la modalità di trackball
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN) { moving_trackball = true; }
		if (state == GLUT_UP) { moving_trackball = false; }
		
		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
		break;

	case GLUT_RIGHT_BUTTON:

		//Con il tasto destro si selezionano gli oggetti nella scena
		if (state == GLUT_DOWN && glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			float xmouse = x;
			float ymouse = y;
			vec3 ray_wor = get_ray_from_mouse(xmouse, ymouse);

			selected_obj = -1;
			float closest_intersection = 0.0f;
			for (int i = 0; i < Scena.size(); i++)
			{
				float t_dist = 0.0f;
				//Interseco il raggio che esce dalla camera nella direzione del mouse con la sfera centrata nell'ancora di tutti gli oggetti 
				//posizionati nel mondo per individuare se c'è intersezione con l'oggetto
				if (ray_sphere(ViewSetup.position, ray_wor, Scena[i].ancora_world, raggio_sfera, &t_dist))
				{
					if (selected_obj == -1 || t_dist < closest_intersection)
					{
						selected_obj = i;
						closest_intersection = t_dist;
					}
				}
			}
			if (selected_obj > -1)
				printf("Oggetto selezionato %d -> %s \n", selected_obj, Scena[selected_obj].nome.c_str());
		}
		break;
	default:
		break;
	}
	
	glutPostRedisplay();

}

vec3 getTrackBallPoint(float x, float y)
{
	//Dalla posizione del mouse al punto proiettato sulla semisfera con centro l'origine e raggio 1

	float Delta, tmp;
	vec3 point;
	//map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	//Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
	Delta = 1.0f -tmp ;
	if (Delta > 0.0f)
		point.z = sqrt(Delta);
	else
		point.z =0;

	return point;
}
void mouseActiveMotion(int x, int y)
{
	float pi = glm::pi<float>();
	// Spostamento tramite  trackball del vettore posizione Camera 
	if (!moving_trackball) {
		return;
	}
	vec3 corrente = normalize(getTrackBallPoint(x, y));
	vec3 precedente = normalize(getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y));
	
	vec3 rotation_vec = cross(precedente,corrente);

	//Trasformiano l'angolo in gradi
	float theta = (acos(dot(precedente, corrente)))*(180 / pi);

	if (isnan(theta)) {
		return;
	}

	//Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
	quat rotation = angleAxis(theta, rotation_vec);
	//Trasformiamo il quaternione in matrice di rotazione
	mat4 rotation_matrix = toMat4(rotation);
	//Aggiorniamo direzione e posizione della telecamera
	ViewSetup.direction = ViewSetup.position - ViewSetup.target;
	ViewSetup.position = ViewSetup.target +  rotation_matrix*ViewSetup.direction;

	//Memorizzo l' ultima posizione del mouse

	last_mouse_pos_X = x; 
	
	last_mouse_pos_Y = y;
	
	glutPostRedisplay();

}
void resize(int w, int h)
{
	//Imposto la matrice di Proiezione per il rendering del testo

	Projection_text = ortho(0.0f, (float)width, 0.0f, (float)height);

	//Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ciò che è nel mondo
	 //Se l'aspect ratio del mondo è diversa da quella della finestra devo mappare in modo diverso 
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


void my_passive_mouse(int xpos, int ypos)
{


	float alfa = 0.05; //serve ridimensionare l'offset tra due posizioni successive del mosue
	ypos = height - ypos;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	xoffset *= alfa;
	yoffset *= alfa;
	Theta += xoffset;   //Aggiorno l'angolo Theta sommandogli l'offset della posizione x del mouse
	Phi += yoffset;  //Aggiorno l'angolo Phi sommandogli l'offset della posizione y del mouse 

	// Facciamo si' che l'angolo di Phi vari tra -90 e 90.
	if (Phi > 89.0f)
		Phi = 89.0f;
	if (Phi < -89.0f)
		Phi = -89.0f;

	//Ricavo le coordinate x,y,z del punto che sulla sfera individuato dagli angoli Theta e Phi
	vec3 front;
	front.x = cos( radians(Theta)) * cos( radians(Phi));
	front.y = sin( radians(Phi));    
	front.z = sin( radians(Theta)) * cos( radians(Phi));
	//Considero la direzione normalizzata (nota la quarta componente uguale a 0 indica una direzione
	ViewSetup.direction = vec4(normalize(front), 0.0); //Aggiorno la direzione della telecamera
	ViewSetup.target = ViewSetup.position + ViewSetup.direction; //aggiorno il punto in cui guarda la telecamera
 
	glutPostRedisplay();
}

void drawScene(void)
{


	//Utilizzo il program shader per il disegno

	glUseProgram(programId);

	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
	
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));


	glClearColor(0.0, 0.0, 0.0, 1.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  /*
	  Disegno Sfondo
	  */
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	resolution.x = (float)width;
	resolution.y = (float)height;
	glUseProgram(programId_Sfondo);
	//Trasferisco le variabili uniformi allo shader sfondo: tempo, risoluzione e posizione del mouse, Matrice di Proiezione e Matrice di Vista, Matrice di Modellazione
	 glUniform1f(loc_timeS, time);
	 glUniform2f(loc_res, resolution.x, resolution.y);
     glUniform2f(loc_mouse, mousepos.x, mousepos.y);

	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));

	glUniformMatrix4fv(MatModelS, 1, GL_FALSE, value_ptr(Scena[0].Model));
	glBindVertexArray(Scena[0].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[0].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	 
	//Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	
	//Passo al Vertex Shader il puntatore alla matrice View, che sarà associata alla variabile Uniform mat4 Projection
   //all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);

	glUseProgram(programId);


	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));
	for (int k = 1; k < Scena.size(); k++)
	{
		//Trasformazione delle coordinate dell'ancora dal sistema di riferimento dell'oggetto in sistema
		//di riferimento del mondo premoltiplicando per la matrice di Modellazione.

		Scena[k].ancora_world = Scena[k].ancora_obj;
		Scena[k].ancora_world = Scena[k].Model * Scena[k].ancora_world;
		//Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sarà associata alla variabile Uniform mat4 Projection
		//all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glBindVertexArray(Scena[k].VAO);
		
	 	if (visualizzaAncora==TRUE)
		{
			//Visualizzo l'ancora dell'oggetto
			int ind = Scena[k].indici.size() - 1;

		
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, (Scena[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

	 
		glBindVertexArray(0);

		for (int i = -4; i < 5; i++) {
			posyN = i * 10;

			for (int j = -4; j < 5; j++) {
				posxN = j * 10;

				for (int k = 0; k < Snowman.size(); k++) {
					mat4 Model = mat4(1.0);

					if (k == Snowman.size() - 2)
					{
						Model = translate(Model, vec3(-1.0, 0.5, 0.0));
						Model = rotate(Model, radians(angolo), vec3(0.0, 0.0, 1.0));
						Model = scale(Model, vec3(1, 0.25, 0.25));
						Model = translate(mat4(1.0), vec3(posxN, 0.0, posyN)) * Model;
					}
					else
						Model = translate(Model, vec3(posxN, 0.0, posyN)) * Snowman[k].Model;


					glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Model));
					glBindVertexArray(Snowman[k].VAO);
					//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					glDrawElements(GL_TRIANGLES, (Snowman[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);

				}
			}
		}
	}

	//Disegno Bandiera

	for (int k = 0; k < Bandiera.size(); k++)
	{
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Bandiera[k].Model));
		printf("Bandiera %d  sceltaVS = %d \n", k, Bandiera[k].sceltaVS);
		//glUniform1i(loc_sceltaVs, Bandiera[k].sceltaVS);
		glBindVertexArray(Bandiera[k].VAO);
		glDrawElements(GL_TRIANGLES, (Bandiera[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
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
void update(int a)
{
	angolo += 0.05;


	if (angolo >= 0 && angolo < 25.0)
		angolo += 0.05;
	if (angolo > 25)
		angolo = 0;

	glutTimerFunc(10, update, 0);
	glutPostRedisplay();

}


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
	
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
	//glutPassiveMotionFunc(my_passive_mouse);
	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glutTimerFunc(10, update, 0);
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


	
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");

	MatrixProjS = glGetUniformLocation(programId_Sfondo, "Projection");
	//Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	//QUesto identificativo sarà poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModelS = glGetUniformLocation(programId_Sfondo, "Model");

	MatViewS = glGetUniformLocation(programId_Sfondo, "View");

	loc_timeS = glGetUniformLocation(programId_Sfondo, "timeS");

	loc_res = glGetUniformLocation(programId_Sfondo, "resolution");

	loc_mouse = glGetUniformLocation(programId_Sfondo, "mouse");

	glutMainLoop();
}

