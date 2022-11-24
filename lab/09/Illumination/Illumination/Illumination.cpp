#include <iostream>

#include "ShaderMaker.h"
#include "Lib.h"
#include "geometria.h"
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
bool visualizzaAncora = FALSE;
int width = 1024;
int height = 800;

// Gestione Mouse

static int last_mouse_pos_Y;
static int last_mouse_pos_X;
static bool moving_trackball = 0;

bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;

// Contatore per l'aggiunta di cubie piramidi
int cont_cubi = 0, cont_pir = 0;

// Varibili per il reshape

int w_up = width;
int h_up = height;
// Matrice di Porjection Ortogonale per il testo

mat4 Projection_text;

string stringa_asse;
float cameraSpeed = 0.1;

// variabili per la comunicazione delle variabili uniformi con gli shader
static unsigned int programId, programId_text, programId_Sfondo, MatrixProj, MatrixProj_txt, MatModel, MatView;
static unsigned int MatrixProjS, MatViewS, MatModelS, loc_timeS, loc_res, loc_mouse, loc_time, lsceltaFS, lsceltaVS;
static unsigned int loc_view_pos;

// VAO e VBO per la gestione dek testo
unsigned int VAO_Text, VBO_Text;

float raggio_sfera = 2.5;

string Operazione;
vec3 asse = vec3(0.0, 1.0, 0.0);
int selected_obj = 0;
Mesh Cubo, Piano, Piramide;
vec2 resolution, mousepos;
float posxN = -5, posyN = 0.0;
float angolo = 0.0;

float Theta = -90.0f;
float Phi = 0.0f;

mat4 Projection, Model, View;
typedef enum
{
	WIRE_FRAME,
	FACE_FILL,
	CULLING_ON,
	CULLING_OFF,
} MenuOption;

enum
{
	NAVIGATION,
	CAMERA_MOVING,
	TRASLATING,
	ROTATING,
	SCALING
} OperationMode;
enum
{
	X,
	Y,
	Z
} WorkingAxis;

// La luce ha posizione, colore e potenza
point_light light;
static vector<Material> materials;
static vector<Shader> shaders;

// Location di tutte le variabili uniformi dei materiali
LightShaderUniform light_unif = {};

// MATERIALI DI BASE, definiti come variabili vec3
glm::vec3 red_plastic_ambient = {0.1, 0.0, 0.0},
		  red_plastic_diffuse = {0.6, 0.1, 0.1},
		  red_plastic_specular = {0.7, 0.6, 0.6};
GLfloat red_plastic_shininess = 150.0f;

glm::vec3 brass_ambient = {0.5, 0.06, 0.015},
		  brass_diffuse = {0.78, 0.57, 0.11},
		  brass_specular = {0.99, 0.91, 0.81};
GLfloat brass_shininess = 27.8f;

glm::vec3 emerald_ambient = {0.0215, 0.04745, 0.0215},
		  emerald_diffuse = {0.07568, 0.61424, 0.07568},
		  emerald_specular = {0.633, 0.727811, 0.633};
GLfloat emerald_shininess = 78.8f;

glm::vec3 snow_white_ambient = {0.2, 0.2, 0.2},
		  snow_white_diffuse = {0.95, 0.96, 0.98},
		  snow_white_specular{0.8, 0.8, 0.8};
GLfloat snow_white_shininess = 1.78125f;

glm::vec3 yellow_ambient = {0.8, 0.8, 0.0},
		  yellow_diffuse = {1.0, 1.0, 0.6},
		  yellow_specular{0.9, 0.9, 0.04};
GLfloat yellow_shininess = 1.78125f;

glm::vec3 rosa_ambient = {0.05f, 0.0f, 0.0f},
		  rosa_diffuse = {0.5f, 0.4f, 0.4f},
		  rosa_specular{0.7f, 0.04f, 0.04f};
GLfloat rosa_shininess = 1.78125f;

glm::vec3 marrone_ambient = {0.19125f, 0.0735f, 0.0225f},
		  marrone_diffuse = {0.7038f, 0.27048f, 0.0828f},
		  marrone_specular{0.256777f, 0.137622f, 0.086014f};
GLfloat marrone_shininess = 12.8f;

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char *vertexShader = (char *)"vertexShader_C_S.glsl";
	char *fragmentShader = (char *)"fragmentShader_C_S.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	// Generazione del program shader per la gestione del testo
	vertexShader = (char *)"VertexShader_Text.glsl";
	fragmentShader = (char *)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);

	vertexShader = (char *)"vertexShader_C_S.glsl";
	fragmentShader = (char *)"fragmentShader_Sfondo.glsl";

	programId_Sfondo = ShaderMaker::createProgram(vertexShader, fragmentShader);
}

void INIT_Illuminazione()
{
	// Setup della luce in coordinate del mondo, al centro in alto
	light.position = {0.0, 10.0, 0.0};
	light.color = {1.0, 1.0, 1.0};
	light.power = 2.f;

	// Setup dei materiali
	//  Materials setup
	materials.resize(8);
	materials[MaterialType::RED_PLASTIC].name = "Red Plastic";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "Emerald";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "Brass";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SNOW_WHITE].name = "Snow_White";
	materials[MaterialType::SNOW_WHITE].ambient = snow_white_ambient;
	materials[MaterialType::SNOW_WHITE].diffuse = snow_white_diffuse;
	materials[MaterialType::SNOW_WHITE].specular = snow_white_specular;
	materials[MaterialType::SNOW_WHITE].shininess = snow_white_shininess;

	materials[MaterialType::YELLOW].name = "Yellow";
	materials[MaterialType::YELLOW].ambient = yellow_ambient;
	materials[MaterialType::YELLOW].diffuse = yellow_diffuse;
	materials[MaterialType::YELLOW].specular = yellow_specular;
	materials[MaterialType::YELLOW].shininess = yellow_shininess;

	materials[MaterialType::ROSA].name = "ROSA";
	materials[MaterialType::ROSA].ambient = rosa_ambient;
	materials[MaterialType::ROSA].diffuse = rosa_diffuse;
	materials[MaterialType::ROSA].specular = rosa_specular;
	materials[MaterialType::ROSA].shininess = rosa_shininess;

	materials[MaterialType::MARRONE].name = "MARRONE";
	materials[MaterialType::MARRONE].ambient = marrone_ambient;
	materials[MaterialType::MARRONE].diffuse = marrone_diffuse;
	materials[MaterialType::MARRONE].specular = marrone_specular;
	materials[MaterialType::MARRONE].shininess = marrone_shininess;
	materials[MaterialType::NO_MATERIAL].name = "NO_MATERIAL";
	materials[MaterialType::NO_MATERIAL].ambient = glm::vec3(1, 1, 1);
	materials[MaterialType::NO_MATERIAL].diffuse = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].specular = glm::vec3(0, 0, 0);
	materials[MaterialType::NO_MATERIAL].shininess = 1.f;

	// Setup degli shader
	shaders.resize(5);
	// Usiamo solo il colore dell'oggetto
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";
	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";
	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "PHONG SHADING";
	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "ONDE SHADING";
	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "BANDIERA SHADING";
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

	// Genero , rendo attivo, riempio il VBO delle normali
	//  Per lavorare con i modelli di illuminazione si lavora con le normali
	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	// Adesso carico il VBO delle NORMALI nel layer 2
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(2);

	// EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void INIT_VAO(void)
{
	Mesh Sfondo, Sfera, Cono, Cilindro, Toro;

	crea_piano(&Sfondo, vec4(0.2, 0.2, 0.9, 1.0));
	crea_VAO_Vector(&Sfondo);
	Sfondo.nome = "Sfondo";
	Sfondo.Model = mat4(1.0);
	Sfondo.Model = translate(Sfondo.Model, vec3(0.0, 0.0, -100.0));
	Sfondo.Model = scale(Sfondo.Model, vec3(5000.0f, 5000.00f, 1.0f));
	Sfondo.Model = rotate(Sfondo.Model, radians(90.0f), vec3(1.0, 0.0, 0.0));
	Sfondo.sceltaVS = 0;
	Sfondo.sceltaFS = 1;
	Scena.push_back(Sfondo);

	// Piano
	crea_piano_suddiviso(&Piano, vec4(0.9, 0.9, 0.9, 1.0));
	crea_VAO_Vector(&Piano);
	Piano.nome = "Terreno";
	// definizione della matrice di Modellazione
	Piano.Model = mat4(1.0);
	Piano.Model = translate(Piano.Model, vec3(0.0, -5.0, 0.0));
	Piano.Model = scale(Piano.Model, vec3(500.0f, 1.0f, 500.0f));
	Piano.sceltaVS = 1;
	Piano.sceltaFS = 1;
	Piano.material = MaterialType::SNOW_WHITE;
	Scena.push_back(Piano);

	// Sfera
	crea_sfera(&Sfera, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Sfera);
	Sfera.Model = mat4(1.0);
	Sfera.Model = translate(Sfera.Model, vec3(-6.0, -1.5, 0.0));
	Sfera.Model = scale(Sfera.Model, vec3(1.0f, 1.0f, 1.0f));
	Sfera.sceltaVS = 1;
	Sfera.sceltaFS = 1;
	Sfera.material = MaterialType::EMERALD;
	Scena.push_back(Sfera);

	// cono
	crea_cono(&Cono, vec4(1.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Cono);
	Cono.nome = "Cono";
	Cono.Model = mat4(1.0);
	Cono.Model = translate(Cono.Model, vec3(5.0, -1.5, 0.0));
	Cono.Model = scale(Cono.Model, vec3(1.2f, 1.5f, 1.2f));
	Cono.Model = rotate(Cono.Model, radians(180.0f), vec3(1.0, 0.0, 0.0));
	Cono.sceltaVS = 1;
	Cono.sceltaFS = 1;
	Cono.material = MaterialType::RED_PLASTIC;
	Scena.push_back(Cono);

	// CILIDNRO
	crea_cilindro(&Cilindro, vec4(1.0, 0.0, 1.0, 1.0));
	crea_VAO_Vector(&Cilindro);
	Cilindro.Model = mat4(1.0);
	Cilindro.Model = translate(Cilindro.Model, vec3(-2.0, -1.5, 0.0));
	Cilindro.Model = scale(Cilindro.Model, vec3(1.0, 4.5, 1.0));
	Cilindro.nome = "Cilindro";
	Cilindro.material = MaterialType::MARRONE;
	Cilindro.sceltaVS = 1;
	Cilindro.sceltaFS = 1;
	Scena.push_back(Cilindro);

	// TORO
	crea_toro(&Toro, vec4(0.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Toro);
	Toro.Model = mat4(1.0);
	Toro.Model = translate(Toro.Model, vec3(4.0, 0.0, 0.0));
	Toro.Model = scale(Toro.Model, vec3(1.0, 1.0, 1.0));
	Toro.nome = "Toro";
	Toro.material = MaterialType::BRASS;
	Toro.sceltaVS = 1;
	Toro.sceltaFS = 1;
	Scena.push_back(Toro);
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
	// ricordare che mat4(1) costruisce una matrice identit� di ordine 4
	mat4 traslation = glm::translate(glm::mat4(1), translation_vector);
	mat4 scale = glm::scale(glm::mat4(1), glm::vec3(scale_factor, scale_factor, scale_factor));
	mat4 rotation = glm::rotate(glm::mat4(1), angle, rotation_vector);

	// Modifica la matrice di Modellazione dell'oggetto della scena selezionato postmolitplicando per le matrici scale*rotarion*traslation
	Scena[selected_obj].Model = Scena[selected_obj].Model * scale * rotation * traslation;

	glutPostRedisplay();
}

// Movimento della telecamera
void moveCameraForward()
{
	vec4 direction = ViewSetup.target - ViewSetup.position; // Direzione lungo cui si sposta la telecamera in coordinate del mondo
	ViewSetup.position += direction * cameraSpeed;
}

void moveCameraBack()
{
	vec4 direction = ViewSetup.target - ViewSetup.position; // Direzione lungo cui si sposta la telecamera in coordinate del mondo
	ViewSetup.position -= direction * cameraSpeed;
}

void moveCameraLeft()
{
	vec3 direction = ViewSetup.target - ViewSetup.position; // Direzione lungo cui si sposta la telecamera in coordinate del mondo
	vec3 slide_vector = cross(direction, vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position -= vec4(slide_vector, .0);
	ViewSetup.target -= vec4(slide_vector, .0);
}

void moveCameraRight()
{
	vec3 direction = ViewSetup.target - ViewSetup.position; // Direzione lungo cui si sposta la telecamera in coordinate del mondo
	vec3 slide_vector = cross(direction, vec3(ViewSetup.upVector)) * cameraSpeed;
	ViewSetup.position += vec4(slide_vector, .0);
	ViewSetup.target += vec4(slide_vector, .0);
}

void moveCameraUp()
{
	vec3 direction = ViewSetup.target - ViewSetup.position; // Direzione lungo cui si sposta la telecamera in coordinate del mondo
	vec3 slide_vector = normalize(glm::cross(direction, vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(direction, slide_vector) * cameraSpeed;
	ViewSetup.position -= vec4(upDirection, 0.0);
	ViewSetup.target -= vec4(upDirection, 0.0);
}

void moveCameraDown()
{
	vec4 direction = ViewSetup.target - ViewSetup.position;
	vec3 slide_vector = normalize(cross(vec3(direction), vec3(ViewSetup.upVector)));
	vec3 upDirection = cross(vec3(direction), slide_vector) * cameraSpeed;
	ViewSetup.position += vec4(upDirection, 0.0);
	ViewSetup.target += vec4(upDirection, 0.0);
}

// Gestione eventi tastiera per il movimento della telecamera
void keyboardPressedEvent(unsigned char key, int x, int y)
{
	char *intStr;
	string str;
	switch (key)
	{

	case 'a':
		moveCameraLeft();
		break;

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

	case 'v': // Visualizzazione ancora
		visualizzaAncora = TRUE;
		break;
	case 'C': // Si inserisce un cubo
		crea_cubo(&Cubo);
		crea_VAO_Vector(&Cubo);
		cont_cubi += 1;
		str = std::to_string(cont_cubi);

		Cubo.nome = "Cubo" + str;
		Cubo.Model = mat4(1.0);
		Cubo.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
		Cubo.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
		Cubo.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
		Cubo.material = MaterialType::BRASS;
		Cubo.sceltaVS = 1;
		Cubo.sceltaFS = 1;
		Scena.push_back(Cubo);
		break;
	case 'P': // Si inserisce una piramide
		crea_piramide(&Piramide);
		crea_VAO_Vector(&Piramide);
		cont_pir += 1;
		str = std::to_string(cont_pir);

		Piramide.nome = "Piramide" + str;
		Piramide.Model = mat4(1.0);
		Piramide.Model = translate(Cubo.Model, vec3(3.5, 0.5, 2.5));
		Piramide.Model = scale(Cubo.Model, vec3(2.0f, 2.0f, 2.0f));
		Piramide.Model = translate(Cubo.Model, vec3(-0.5, 0.0, -0.5));
		Piramide.material = MaterialType::ROSA;
		Piramide.sceltaVS = 0;
		Piramide.sceltaFS = 0;
		Scena.push_back(Piramide);
		break;
	case 'g': // Si entra in modalit� di operazione traslazione
		OperationMode = TRASLATING;
		Operazione = "TRASLAZIONE";
		break;
	case 'r': // Si entra in modalit� di operazione rotazione
		OperationMode = ROTATING;
		Operazione = "ROTAZIONE";
		break;
	case 'S':
		OperationMode = SCALING; // Si entra in modalit� di operazione scalatura
		Operazione = "SCALATURA";
		break;
	case 27:
		glutLeaveMainLoop();
		break;
		// Selezione dell'asse
	case 'x':
		WorkingAxis = X; // Seleziona l'asse X come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse X";
		break;
	case 'y':
		WorkingAxis = Y; // Seleziona l'asse Y come asse lungo cui effettuare l'operazione selezionata (tra traslazione, rotazione, scalatura)
		stringa_asse = " Asse Y";
		break;
	case 'z':
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

	// I tasti + e -  aggiornano lo spostamento a destra o a sinistra, la rotazione in segno antiorario o in senso orario, la scalatura come amplificazione o diminuizione delle dimensioni

	float amount = .01;
	if (key == '+')
		amount *= 1;

	if (key == '-')
		amount *= -1;

	switch (OperationMode)
	{

		// la funzione modifyModelMatrix(glm::vec3 translation_vector, glm::vec3 rotation_vector, GLfloat angle, GLfloat scale_factor)
		//  definisce la matrice di modellazione che si vuole postmoltiplicare alla matrice di modellazione dell'oggetto selezionato, per poterlo traslare, ruotare scalare.

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
	// Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = glm::vec4(0.0, 0.5, 15.0, 0.0);
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

vec3 get_ray_from_mouse(float mouse_x, float mouse_y)
{

	mouse_y = height - mouse_y;

	// mappiamo le coordinate di viewport del mouse [0,width], [0,height] in coordinate normalizzate [-1,1]
	float x = (2.0f * mouse_x) / width - 1.0;
	float y = (2.0f * mouse_y) / height - 1.0;
	float z = 1.0f;
	vec3 ray_nds = vec3(x, y, z);
	// Nel clip space hanno coordinate: nota bene la terza coordinata uguale a -1
	vec4 ray_clip = vec4(x, y, -1.0, 1.0);

	// Le coordinate Nell' eye space si ottengono premoltiplicando per l'inversa della matrice Projection.

	vec4 ray_eye = inverse(Projection) * ray_clip;

	// Direzione del raggio
	ray_eye = vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

	// le coordinate nel world space: si ottengono premoltiplicando per l'inversa della matrice di Vista
	vec3 ray_wor = vec3(inverse(View) * ray_eye);

	ray_wor = normalize(ray_wor);

	return ray_wor;
}

/*controlla se un raggio interseca una sfera. In caso negativo, restituisce false. Rigetta
le intersezioni dietro l'origine del raggio, e pone  intersection_distance all'intersezione p i� vicina.
*/

bool ray_sphere(vec3 ray_origin_wor, vec3 ray_direction_wor, vec3 sphere_centre_wor, float sphere_radius, float *intersection_distance)
{

	// Calcoliamo O-C
	vec3 dist_sfera = ray_origin_wor - sphere_centre_wor;
	float b = dot(dist_sfera, ray_direction_wor);
	float cc = dot(dist_sfera, dist_sfera) - sphere_radius * sphere_radius;

	float delta = b * b - cc;

	if (delta < 0) // Il raggio non interseca la sfera
		return false;
	// Calcolo i valori di t per cui il raggio interseca la sfera e restituisco il valore dell'intersezione
	// pi� vicina all'osservatore (la t pi� piccola)
	if (delta > 0.0f)
	{
		// calcola le due intersezioni
		float t_a = -b + sqrt(delta);
		float t_b = -b - sqrt(delta);
		*intersection_distance = t_b;

		// Caso di intersezioni dietro l'osservatore
		if (t_a < 0.0)
		{
			if (t_b < 0)
				return false;
		}

		return true;
	}
	// Caso in cui il raggio � tangente alla sfera: un interesezione con molteplicit� doppia.
	if (delta == 0)
	{
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

	switch (button)
	{
		// Con il tasto sinistro premuto si attiva la modalit� di trackball
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			moving_trackball = true;
		}
		if (state == GLUT_UP)
		{
			moving_trackball = false;
		}

		last_mouse_pos_X = x;
		last_mouse_pos_Y = y;
		break;

	case GLUT_RIGHT_BUTTON:

		// Con il tasto destro si selezionano gli oggetti nella scena
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
				// Interseco il raggio che esce dalla camera nella direzione del mouse con la sfera centrata nell'ancora di tutti gli oggetti
				// posizionati nel mondo per individuare se c'� intersezione con l'oggetto
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
	// Dalla posizione del mouse al punto proiettato sulla semisfera con centro l'origine e raggio 1

	float Delta, tmp;
	vec3 point;
	// map to [-1;1]
	point.x = (2.0f * x - width) / width;
	point.y = (height - 2.0f * y) / height;

	// Cooordinata z del punto di coordinate (x,y,z) che si muove sulla sfera virtuale con centro (0,0,0) e raggio r=1
	tmp = pow(point.x, 2.0) - pow(point.y, 2.0);
	Delta = 1.0f - tmp;
	if (Delta > 0.0f)
		point.z = sqrt(Delta);
	else
		point.z = 0;

	return point;
}
void mouseActiveMotion(int x, int y)
{
	float velocity = 40;
	float pi = glm::pi<float>();
	// Spostamento tramite  trackball del vettore posizione Camera
	if (!moving_trackball)
	{
		return;
	}
	vec3 corrente = normalize(getTrackBallPoint(x, y));
	vec3 precedente = normalize(getTrackBallPoint(last_mouse_pos_X, last_mouse_pos_Y));

	vec3 rotation_vec = cross(precedente, corrente);

	// Calcoliamo il coseno dell'angolo tra il segmento che collega l'origine e il click corrente e il segmento
	// che collega l'origine ed il click corrente
	float cosangolo = dot(precedente, corrente);

	// Il prodotto scalare tra due vettori normalizzati che hanno lunghezza 1,
	//  pu� variare tra -1 ed 1.
	// Per problemi numerici dovuti al fatto che lavoriamo con i numeri finiti, per evitare che acos
	//  dia risulato nan, nel caso in cui cosangolo risulti maggiore di 1 rimettiamolo ad 1
	if (cosangolo > 1)
		cosangolo = 1.0;
	// nel caso in cui cosangolo risulti minore di -1 rimettiamolo a -1
	if (cosangolo < -1)
		cosangolo = -1.0;

	// Calcoliamo l'angolo utilizzando l'arcocoseno
	float theta = (acos(cosangolo)) * velocity;

	// Costruiamo il quaternione a partire dall'angolo di rotazione e dalla direzione lungo cui ruotare
	quat rotation = angleAxis(theta, rotation_vec);
	// Trasformiamo il quaternione in matrice di rotazione
	mat4 rotation_matrix = toMat4(rotation);
	// Aggiorniamo direzione e posizione della telecamera
	ViewSetup.direction = ViewSetup.position - ViewSetup.target;
	ViewSetup.position = ViewSetup.target + rotation_matrix * ViewSetup.direction;

	// Memorizzo l' ultima posizione del mouse

	last_mouse_pos_X = x;

	last_mouse_pos_Y = y;

	glutPostRedisplay();
}
void resize(int w, int h)
{
	// Imposto la matrice di Proiezione per il rendering del testo

	Projection_text = ortho(0.0f, (float)width, 0.0f, (float)height);

	// Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	float AspectRatio_mondo = (float)(width) / (float)(height); // Rapporto larghezza altezza di tutto ci� che � nel mondo
																// Se l'aspect ratio del mondo � diversa da quella della finestra devo mappare in modo diverso
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

void my_passive_mouse(int xpos, int ypos)
{

	float alfa = 0.05; // serve ridimensionare l'offset tra due posizioni successive del mosue
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
	Theta += xoffset; // Aggiorno l'angolo Theta sommandogli l'offset della posizione x del mouse
	Phi += yoffset;	  // Aggiorno l'angolo Phi sommandogli l'offset della posizione y del mouse

	// Facciamo si' che l'angolo di Phi vari tra -90 e 90.
	if (Phi > 89.0f)
		Phi = 89.0f;
	if (Phi < -89.0f)
		Phi = -89.0f;

	// Ricavo le coordinate x,y,z del punto che sulla sfera individuato dagli angoli Theta e Phi
	vec3 front;
	front.x = cos(radians(Theta)) * cos(radians(Phi));
	front.y = sin(radians(Phi));
	front.z = sin(radians(Theta)) * cos(radians(Phi));
	// Considero la direzione normalizzata (nota la quarta componente uguale a 0 indica una direzione
	ViewSetup.direction = vec4(normalize(front), 0.0);			 // Aggiorno la direzione della telecamera
	ViewSetup.target = ViewSetup.position + ViewSetup.direction; // aggiorno il punto in cui guarda la telecamera

	glutPostRedisplay();
}

void drawScene(void)
{

	// Utilizzo il program shader per il disegno

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
	// Trasferisco le variabili uniformi allo shader sfondo: tempo, risoluzione e posizione del mouse, Matrice di Proiezione e Matrice di Vista, Matrice di Modellazione
	glUniform1f(loc_timeS, time);
	glUniform2f(loc_res, resolution.x, resolution.y);
	glUniform2f(loc_mouse, mousepos.x, mousepos.y);

	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));

	glUniformMatrix4fv(MatModelS, 1, GL_FALSE, value_ptr(Scena[0].Model));
	glBindVertexArray(Scena[0].VAO);
	glDrawElements(GL_TRIANGLES, (Scena[0].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.

	// Passo al Vertex Shader il puntatore alla matrice View, che sarà associata alla variabile Uniform mat4 Projection
	// all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);

	// Cambio program per renderizzare tutto il resto della scena
	glUseProgram(programId);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	// Passo allo shader il puntatore a  colore luce, posizione ed intensita'
	glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	// Passo allo shader il puntatore alla posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);

	for (int k = 1; k < Scena.size(); k++)
	{
		// Trasformazione delle coordinate dell'ancora dal sistema di riferimento dell'oggetto in sistema
		// di riferimento del mondo premoltiplicando per la matrice di Modellazione.

		Scena[k].ancora_world = Scena[k].ancora_obj;
		Scena[k].ancora_world = Scena[k].Model * Scena[k].ancora_world;
		// Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sar� associata alla variabile Uniform mat4 Projection
		// all'interno del Vertex shader. Uso l'identificatio MatModel

		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glUniform1i(lsceltaVS, Scena[k].sceltaVS);
		glUniform1i(lsceltaFS, Scena[k].sceltaFS);
		// Passo allo shader il puntatore ai materiali
		glUniform3fv(light_unif.material_ambient, 1, glm::value_ptr(materials[Scena[k].material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, glm::value_ptr(materials[Scena[k].material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, glm::value_ptr(materials[Scena[k].material].specular));
		glUniform1f(light_unif.material_shininess, materials[Scena[k].material].shininess);

		glBindVertexArray(Scena[k].VAO);

		if (visualizzaAncora == TRUE)
		{
			// Visualizzo l'ancora dell'oggetto
			int ind = Scena[k].indici.size() - 1;

			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawElements(GL_TRIANGLES, (Scena[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);

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

void update(int a)
{
	angolo += 0.5;
	glutTimerFunc(10, update, 0);
	glutPostRedisplay();
}

void main_menu_func(int option)
{
	switch (option)
	{
	case MenuOption::WIRE_FRAME:
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case MenuOption::FACE_FILL:
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	default:
		break;
	}
}

void material_menu_function(int option)
{
	if (selected_obj > -1)
		Scena[selected_obj].material = (MaterialType)option;
}

void shader_menu_function(int option)
{
	if (selected_obj > -1)
		Scena[selected_obj].sceltaVS = shaders[option].value;
}

void buildOpenGLMenu()
{
	int materialSubMenu = glutCreateMenu(material_menu_function);

	glutAddMenuEntry(materials[MaterialType::EMERALD].name.c_str(), MaterialType::EMERALD);
	glutAddMenuEntry(materials[MaterialType::BRASS].name.c_str(), MaterialType::BRASS);
	glutAddMenuEntry(materials[MaterialType::SNOW_WHITE].name.c_str(), MaterialType::SNOW_WHITE);
	glutAddMenuEntry(materials[MaterialType::YELLOW].name.c_str(), MaterialType::YELLOW);
	glutAddMenuEntry(materials[MaterialType::ROSA].name.c_str(), MaterialType::ROSA);
	glutAddMenuEntry(materials[MaterialType::MARRONE].name.c_str(), MaterialType::MARRONE);

	int shaderSubMenu = glutCreateMenu(shader_menu_function);
	glutAddMenuEntry(shaders[ShaderOption::NONE].name.c_str(), ShaderOption::NONE);
	glutAddMenuEntry(shaders[ShaderOption::GOURAD_SHADING].name.c_str(), ShaderOption::GOURAD_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::PHONG_SHADING].name.c_str(), ShaderOption::PHONG_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::ONDE_SHADING].name.c_str(), ShaderOption::ONDE_SHADING);
	glutAddMenuEntry(shaders[ShaderOption::BANDIERA_SHADING].name.c_str(), ShaderOption::BANDIERA_SHADING);

	glutCreateMenu(main_menu_func);	 // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga
	glutAddMenuEntry("", -1);
	glutAddMenuEntry("Wireframe", MenuOption::WIRE_FRAME);
	glutAddMenuEntry("Face fill", MenuOption::FACE_FILL);
	glutAddSubMenu("Material", materialSubMenu);
	glutAddSubMenu("Shader", shaderSubMenu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);
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

	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
	// glutPassiveMotionFunc(my_passive_mouse);

	glutMotionFunc(mouseActiveMotion); // Evento tasto premuto
	glutTimerFunc(10, update, 0);
	glewExperimental = GL_TRUE;
	glewInit();

	// Inizializzazione setup illuminazione, materiali
	INIT_Illuminazione();
	// Inizializzazione setup Shader
	INIT_SHADER();
	// Inizializzazione VAO
	INIT_VAO();
	// Inizializzazione setup telecamera
	INIT_CAMERA_PROJECTION();

	// Menu collegato al tasto centrale
	buildOpenGLMenu();

	// Gestione Testo
	INIT_VAO_Text();
	Init_Freetype();

	// Abilita l'uso del Buffer di Profondit� per la gestione dell'eliminazione dlele superifici nascoste
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Projection (in vertex shader).
	// QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Projection al Vertex Shader
	MatrixProj = glGetUniformLocation(programId, "Projection");
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	// QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModel = glGetUniformLocation(programId, "Model");
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 View (in vertex shader)
	// QUesto identificativo sar� poi utilizzato per il trasferimento della matrice View al Vertex Shader
	MatView = glGetUniformLocation(programId, "View");
	loc_time = glGetUniformLocation(programId, "time");
	lsceltaVS = glGetUniformLocation(programId, "sceltaVS");
	lsceltaFS = glGetUniformLocation(programId, "sceltaFS");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");

	// location delle variabili uniformi per la gestione della luce
	// riempimo la struttura del programma con quelle delle strutture
	// del vertex shader che sono state processate.
	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");

	// location delle variabili uniformi per la gestione dei materiali
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");

	// location variabili uniformi per lo shader della gestione dello Sfondo
	MatrixProjS = glGetUniformLocation(programId_Sfondo, "Projection");
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model (in vertex shader)
	// QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Model al Vertex Shader
	MatModelS = glGetUniformLocation(programId_Sfondo, "Model");
	MatViewS = glGetUniformLocation(programId_Sfondo, "View");
	loc_timeS = glGetUniformLocation(programId_Sfondo, "timeS");
	loc_res = glGetUniformLocation(programId_Sfondo, "resolution");
	loc_mouse = glGetUniformLocation(programId_Sfondo, "mouse");

	glutMainLoop();
}
