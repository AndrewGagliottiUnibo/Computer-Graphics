#include <iostream>
#include <windows.h>
#include "AssimpLoader.h"
#include "BaseMaterial.h"
#include "CameraHandler.h"
#include "Enum.h"
#include "Geometries.h"
#include "Lib.h"
#include "MenuHandler.h"
#include "MouseHandler.h"
#include "ShaderMaker.h"
#include "Strutture.h"
#include "TextureHandler.h"


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

int width = 1024;
int height = 800;

// Gestione input utente
static int last_mouse_pos_Y;
static int last_mouse_pos_X;
static bool moving_trackball = false;
bool firstMouse = true;
float lastX = (float)width / 2;
float lastY = (float)height / 2;
float raggio_sfera = 3.0;

// Gestione ancora
bool visualizzaAncora = false;

// Varibili per il reshape
int w_up = width;
int h_up = height;

// Inizializzazione dei modelli
static vector<MeshObj> Model3D;
vector<vector<MeshObj>> ScenaObj;
mat4 Projection, Model, View;
string stringa_asse;

//modifica modelli
int selected_obj = 0;

// variabili per la comunicazione delle variabili uniformi con gli shader
static unsigned int programId, programId_text, programId1, MatrixProj, MatModel, MatView;
static unsigned int lsceltaFS, lsceltaVS, loc_texture, MatViewS, MatrixProjS;
static unsigned int loc_view_pos, MatModelR, MatViewR, MatrixProjR, loc_view_posR, loc_cubemapR;
unsigned int idTex, textureMakima, textureNull, textureSuolo, textureBraccia, cubemapTexture, programIdr;

// Camera
string Operazione;
vec3 asse = vec3(0.0, 1.0, 0.0);
float cameraSpeed = 0.1;

// Locazione file
string meshDir = "Meshes/";
string imageDir = "Textures/";
string skyboxDir = "Skybox/";

/*
	 loads a cubemap texture from 6 individual texture faces
	 order:
	 +X (right)
	 -X (left)
	 +Y (top)
	 -Y (bottom)
	 +Z (front)
	 -Z (back)
*/
vector<string> faces
{
	skyboxDir + "posx.jpg",
	skyboxDir + "negx.jpg",
	skyboxDir + "posy.jpg",
	skyboxDir + "negy.jpg",
	skyboxDir + "posz.jpg",
	skyboxDir + "negz.jpg"
};

// Creazione sfera
float Theta = -90.0f;
float Phi = 0.0f;

// Vettori di materiali e shader
static vector<Material> materials;
static vector<Shader> shaders;

// Luce
float angolo = 0.0;
point_light light;
LightShaderUniform light_unif = {};

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char *vertexShader = (char *)"vertexShader_C.glsl";
	char *fragmentShader = (char *)"fragmentShader_C.glsl";
	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	vertexShader = (char *)"vertexShader_CubeMap.glsl";
	fragmentShader = (char *)"fragmentShader_CubeMap.glsl";
	programId1 = ShaderMaker::createProgram(vertexShader, fragmentShader);

	vertexShader = (char *)"vertexShader_riflessione.glsl";
	fragmentShader = (char *)"fragmentShader_riflessione.glsl";
	programIdr = ShaderMaker::createProgram(vertexShader, fragmentShader);
}

void INIT_Illuminazione()
{
	// Setup della luce
	light.position = {-7.0, 17.0, 12.0};
	light.color = {1.0, 1.0, 1.0};
	light.power = 2.f;

	// Setup dei materiali
	materials.resize(8);
	materials[MaterialType::RED_PLASTIC].name = "RED PLASTIC";
	materials[MaterialType::RED_PLASTIC].ambient = red_plastic_ambient;
	materials[MaterialType::RED_PLASTIC].diffuse = red_plastic_diffuse;
	materials[MaterialType::RED_PLASTIC].specular = red_plastic_specular;
	materials[MaterialType::RED_PLASTIC].shininess = red_plastic_shininess;

	materials[MaterialType::EMERALD].name = "EMERALD";
	materials[MaterialType::EMERALD].ambient = emerald_ambient;
	materials[MaterialType::EMERALD].diffuse = emerald_diffuse;
	materials[MaterialType::EMERALD].specular = emerald_specular;
	materials[MaterialType::EMERALD].shininess = emerald_shininess;

	materials[MaterialType::BRASS].name = "BRASS";
	materials[MaterialType::BRASS].ambient = brass_ambient;
	materials[MaterialType::BRASS].diffuse = brass_diffuse;
	materials[MaterialType::BRASS].specular = brass_specular;
	materials[MaterialType::BRASS].shininess = brass_shininess;

	materials[MaterialType::SNOW_WHITE].name = "WHITE";
	materials[MaterialType::SNOW_WHITE].ambient = snow_white_ambient;
	materials[MaterialType::SNOW_WHITE].diffuse = snow_white_diffuse;
	materials[MaterialType::SNOW_WHITE].specular = snow_white_specular;
	materials[MaterialType::SNOW_WHITE].shininess = snow_white_shininess;

	materials[MaterialType::YELLOW].name = "YELLOW";
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
	shaders[ShaderOption::NONE].value = 0;
	shaders[ShaderOption::NONE].name = "NONE";

	shaders[ShaderOption::GOURAD_SHADING].value = 1;
	shaders[ShaderOption::GOURAD_SHADING].name = "GOURAD SHADING";

	shaders[ShaderOption::PHONG_SHADING].value = 2;
	shaders[ShaderOption::PHONG_SHADING].name = "BLINN-PHONG SHADING INTERPOLATIVO";

	shaders[ShaderOption::ONDE_SHADING].value = 3;
	shaders[ShaderOption::ONDE_SHADING].name = "PHONG SHADING";

	shaders[ShaderOption::BANDIERA_SHADING].value = 4;
	shaders[ShaderOption::BANDIERA_SHADING].name = "CARTOON SHADING";
}

void crea_VAO_Vector_MeshObj(MeshObj *mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	// Genero, rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);

	// Genero , rendo attivo, riempio il VBO delle normali
	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(2);

	// EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void crea_VAO_Vector(Mesh *mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);
	
	// Genero, rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &mesh->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertici.size() * sizeof(vec3), mesh->vertici.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &mesh->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, mesh->colori.size() * sizeof(vec4), mesh->colori.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);

	// Genero , rendo attivo, riempio il VBO delle normali
	glGenBuffers(1, &mesh->VBO_normali);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_normali);
	glBufferData(GL_ARRAY_BUFFER, mesh->normali.size() * sizeof(vec3), mesh->normali.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &mesh->VBO_coord_texture);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO_coord_texture);
	glBufferData(GL_ARRAY_BUFFER, mesh->texCoords.size() * sizeof(vec2), mesh->texCoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(3);

	// EBO di tipo indici
	glGenBuffers(1, &mesh->EBO_indici);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO_indici);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indici.size() * sizeof(GLuint), mesh->indici.data(), GL_STATIC_DRAW);
}

void INIT_VAO(void)
{
	// Cubemap
	Mesh Pannello, Sfondo, Sky;

	// Omino
	Mesh Testa, Cappello, Corpo, Collare, BraccioSX, BraccioDX, ManoSX, ManoDX, GambaDX, GambaSX, Sedere, PiedeDX, PiedeSX;

	// Variables for meshes and texture
	bool obj;
	int nmeshes;

	// texture suolo
	string name = "suolo.jpg";
	string path = imageDir + name;
	textureSuolo = loadTexture(path.c_str(), 0);

	// texture Makima
	name = "makima.jpg";
	path = imageDir + name;
	textureMakima = loadTexture(path.c_str(), 0);

	// texture braccia
	name = "chainsaw.jpg";
	path = imageDir + name;
	textureBraccia = loadTexture(path.c_str(), 0);

	// Sky
	cubemapTexture = loadCubemap(faces, 0);
	crea_cubo(&Sky);
	crea_VAO_Vector(&Sky);
	Scena.push_back(Sky);

	// Disegno omino con il cappello
	// Cono - Cappello riflettente
	crea_cono(&Cappello, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Cappello);
	Cappello.nome = "Cappello";
	Cappello.ModelM = mat4(1.0);
	Cappello.ModelM = translate(Cappello.ModelM, vec3(0.0, 17.0, -14.0));
	Cappello.ModelM = scale(Cappello.ModelM, vec3(3.0f, 3.0f, 3.0f));
	Cappello.ModelM = rotate(Cappello.ModelM, radians(180.0f), vec3(1.0, 0.0, 0.0));
	Cappello.sceltaVS = 1;
	Cappello.sceltaFS = 1;
	Cappello.material = MaterialType::RED_PLASTIC;
	Scena.push_back(Cappello);

	// Sfera - Testa
	crea_sfera(&Testa, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Testa);
	Testa.nome = "Testa";
	Testa.ModelM = mat4(1.0);
	Testa.ModelM = translate(Testa.ModelM, vec3(0.0, 12.8, -14.0));
	Testa.ModelM = scale(Testa.ModelM, vec3(2.8f, 2.8f, 2.8f));
	Testa.sceltaVS = 1;
	Testa.sceltaFS = 1;
	Testa.material = MaterialType::EMERALD;
	Scena.push_back(Testa);

	// Cilindro - Corpo
	crea_cilindro(&Corpo, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Corpo);
	Corpo.nome = "Corpo";
	Corpo.ModelM = mat4(1.0);
	Corpo.ModelM = translate(Corpo.ModelM, vec3(0, 3.8, -14.0));
	Corpo.ModelM = scale(Corpo.ModelM, vec3(2.0, 7.0, 2.0));
	Corpo.material = MaterialType::MARRONE;
	Corpo.sceltaVS = 1;
	Corpo.sceltaFS = 1;
	Scena.push_back(Corpo);

	// Toro - Collare
	crea_toro(&Collare, vec4(0.0, 1.0, 0.0, 1.0));
	crea_VAO_Vector(&Collare);
	Collare.ModelM = mat4(1.0);
	Collare.ModelM = translate(Collare.ModelM, vec3(0.0, 9.8, -14.0));
	Collare.ModelM = scale(Collare.ModelM, vec3(2.0, 2.0, 2.0));
	Collare.nome = "Collare";
	Collare.material = MaterialType::BRASS;
	Collare.sceltaVS = 1;
	Collare.sceltaFS = 1;
	Scena.push_back(Collare);

	// Cilindro  - Braccio dx
	crea_cilindro(&BraccioDX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&BraccioDX);
	BraccioDX.nome = "Braccio dx";
	BraccioDX.ModelM = mat4(1.0);
	BraccioDX.ModelM = translate(BraccioDX.ModelM, vec3(5.0, 5.5, -14.0));
	BraccioDX.ModelM = rotate(BraccioDX.ModelM, radians(45.0f), vec3(0.0, 0.0, 1.0));
	BraccioDX.ModelM = scale(BraccioDX.ModelM, vec3(1.5, 5.0, 1.5));
	BraccioDX.material = MaterialType::MARRONE;
	BraccioDX.sceltaVS = 1;
	BraccioDX.sceltaFS = 1;
	Scena.push_back(BraccioDX);

	// Cilindro - Braccio sx
	crea_cilindro(&BraccioSX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&BraccioSX);
	BraccioSX.nome = "Braccio sx";
	BraccioSX.ModelM = mat4(1.0);
	BraccioSX.ModelM = translate(BraccioSX.ModelM, vec3(-5.0, 5.5, -14.0));
	BraccioSX.ModelM = rotate(BraccioSX.ModelM, radians(-45.0f), vec3(0.0, 0.0, 1.0));
	BraccioSX.ModelM = scale(BraccioSX.ModelM, vec3(1.5, 5.0, 1.5));
	BraccioSX.material = MaterialType::MARRONE;
	BraccioSX.sceltaVS = 1;
	BraccioSX.sceltaFS = 1;
	Scena.push_back(BraccioSX);

	// Sfera - Mano dx
	crea_sfera(&ManoDX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&ManoDX);
	ManoDX.nome = "Mano dx";
	ManoDX.ModelM = mat4(1.0);
	ManoDX.ModelM = translate(ManoDX.ModelM, vec3(5.3, 5.3, -14.0));
	ManoDX.ModelM = scale(ManoDX.ModelM, vec3(1.6f, 1.6f, 1.6f));
	ManoDX.sceltaVS = 1;
	ManoDX.sceltaFS = 1;
	ManoDX.material = MaterialType::EMERALD;
	Scena.push_back(ManoDX);

	// Sfera - Mano sx
	crea_sfera(&ManoSX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&ManoSX);
	ManoSX.nome = "Mano sx";
	ManoSX.ModelM = mat4(1.0);
	ManoSX.ModelM = translate(ManoSX.ModelM, vec3(-5.3, 5.3, -14.0));
	ManoSX.ModelM = scale(ManoSX.ModelM, vec3(1.6f, 1.6f, 1.6f));
	ManoSX.sceltaVS = 1;
	ManoSX.sceltaFS = 1;
	ManoSX.material = MaterialType::EMERALD;
	Scena.push_back(ManoSX);

	// Sfera - Sedere
	crea_sfera(&Sedere, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&Sedere);
	Sedere.nome = "Sedere";
	Sedere.ModelM = mat4(1.0);
	Sedere.ModelM = translate(Sedere.ModelM, vec3(0, 3.5, -14.0));
	Sedere.ModelM = scale(Sedere.ModelM, vec3(2.08f, 2.08f, 2.08f));
	Sedere.sceltaVS = 1;
	Sedere.sceltaFS = 1;
	Sedere.material = MaterialType::EMERALD;
	Scena.push_back(Sedere);

	// Sfera - Piede dx
	crea_sfera(&PiedeDX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&PiedeDX);
	PiedeDX.nome = "Piede dx";
	PiedeDX.ModelM = mat4(1.0);
	PiedeDX.ModelM = translate(PiedeDX.ModelM, vec3(4.8, -1.3, -14.0));
	PiedeDX.ModelM = scale(PiedeDX.ModelM, vec3(1.6f, 1.6f, 1.6f));
	PiedeDX.sceltaVS = 1;
	PiedeDX.sceltaFS = 1;
	PiedeDX.material = MaterialType::EMERALD;
	Scena.push_back(PiedeDX);

	// Sfera - Piede sx
	crea_sfera(&PiedeSX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&PiedeSX);
	PiedeSX.nome = "Piede sx";
	PiedeSX.ModelM = mat4(1.0);
	PiedeSX.ModelM = translate(PiedeSX.ModelM, vec3(-4.8, -1.3, -14.0));
	PiedeSX.ModelM = scale(PiedeSX.ModelM, vec3(1.6f, 1.6f, 1.6f));
	PiedeSX.sceltaVS = 1;
	PiedeSX.sceltaFS = 1;
	PiedeSX.material = MaterialType::EMERALD;
	Scena.push_back(PiedeSX);

	// Cilindro - Gamba dx
	crea_cilindro(&GambaDX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&GambaDX);
	GambaDX.nome = "Gamba dx";
	GambaDX.ModelM = mat4(1.0);
	GambaDX.ModelM = translate(GambaDX.ModelM, vec3(4.5, -1.0, -14.0));
	GambaDX.ModelM = rotate(GambaDX.ModelM, radians(45.0f), vec3(0.0, 0.0, 1.0));
	GambaDX.ModelM = scale(GambaDX.ModelM, vec3(1.5, 5.0, 1.5));
	GambaDX.material = MaterialType::MARRONE;
	GambaDX.sceltaVS = 1;
	GambaDX.sceltaFS = 1;
	Scena.push_back(GambaDX);

	// Cilindro - Gamba sx
	crea_cilindro(&GambaSX, vec4(1.0, 0.0, 0.0, 1.0));
	crea_VAO_Vector(&GambaSX);
	GambaSX.nome = "Gamba sx";
	GambaSX.ModelM = mat4(1.0);
	GambaSX.ModelM = translate(GambaSX.ModelM, vec3(-4.5, -1.0, -14.0));
	GambaSX.ModelM = rotate(GambaSX.ModelM, radians(-45.0f), vec3(0.0, 0.0, 1.0));
	GambaSX.ModelM = scale(GambaSX.ModelM, vec3(1.5, 5.0, 1.5));
	GambaSX.material = MaterialType::MARRONE;
	GambaSX.sceltaVS = 1;
	GambaSX.sceltaFS = 1;
	Scena.push_back(GambaSX);

	// Piano innevato
	crea_piano(&Pannello, vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&Pannello);
	Pannello.nome = "Suolo innevato";
	Pannello.ModelM = mat4(1.0);
	Pannello.ModelM = translate(Pannello.ModelM, vec3(0.0, -6.8, 0.0));
	Pannello.ModelM = scale(Pannello.ModelM, vec3(10000.0f, 10000.0f, 10000.0f));
	Pannello.sceltaVS = 1;
	Pannello.sceltaFS = 1;
	Pannello.material = MaterialType::NO_MATERIAL;
	Scena.push_back(Pannello);

	// Makima panel
	crea_piano(&Pannello, vec4(1.0, 1.0, 1.0, 1.0));
	crea_VAO_Vector(&Pannello);
	Pannello.nome = "Pannello";
	Pannello.ModelM = mat4(1.0);
	Pannello.ModelM = translate(Pannello.ModelM, vec3(0.0, 28.0, -20.0));
	Pannello.ModelM = scale(Pannello.ModelM, vec3(60.0f, 50.0f, 50.0f));
	Pannello.ModelM = rotate(Pannello.ModelM, radians(90.0f), vec3(1.0, 0.0, 0.0));
	Pannello.sceltaVS = 1;
	Pannello.sceltaFS = 1;
	Pannello.material = MaterialType::NO_MATERIAL;
	Scena.push_back(Pannello);

	// Meshes and Texture
	// Pochita
	name = "pochita.obj";
	path = meshDir + name;
	obj = loadAssImp(path.c_str(), Model3D);

	nmeshes = Model3D.size();
	for (int i = 0; i < nmeshes; i++)
	{
		crea_VAO_Vector_MeshObj(&Model3D[i]);
		Model3D[i].ModelM = mat4(1.0);
		Model3D[i].ModelM = translate(Model3D[i].ModelM, vec3(0.0, 2.0, 24.0));
		Model3D[i].ModelM = scale(Model3D[i].ModelM, vec3(8.0, 8.0, 8.0));
		Model3D[i].ModelM = rotate(Model3D[i].ModelM, radians(180.0f), vec3(0.0, 1.0, 0.0));
		Model3D[i].nome = "Pochita";
		Model3D[i].sceltaVS = 1;
		Model3D[i].sceltaFS = 5;

		//le bounding box vengono calcolate per ogni elemento della mesh invece che per la mesh completa
		Model3D[i].boundingBoxMax = TrovaMax(&Model3D[i]);
		Model3D[i].boundingBoxMin = TrovaMin(&Model3D[i]);
	}
	ScenaObj.push_back(Model3D);
	Model3D.clear();

	// Denji - my stolen signature
	name = "chainsawMan.obj";
	path = meshDir + name;
	obj = loadAssImp(path.c_str(), Model3D);

	nmeshes = Model3D.size();
	for (int i = 0; i < nmeshes; i++)
	{
		crea_VAO_Vector_MeshObj(&Model3D[i]);
		Model3D[i].ModelM = mat4(1.0);
		Testa.ModelM = translate(Testa.ModelM, vec3(-13.5, 7.3, 12.0));
		Testa.ModelM = scale(Testa.ModelM, vec3(12.3f, 12.3f, 12.3f));
		Model3D[i].nome = "Denji";
		Model3D[i].sceltaVS = 1;
		Model3D[i].sceltaFS = 5;

		//le bounding box vengono calcolate per ogni elemento della mesh invece che per la mesh completa
		Model3D[i].boundingBoxMax = TrovaMax(&Model3D[i]);
		Model3D[i].boundingBoxMin = TrovaMin(&Model3D[i]);
	}
	ScenaObj.push_back(Model3D);
	Model3D.clear();

	// Case
	// Cottage
	name = "casa.obj";
	path = meshDir + name;
	obj = loadAssImp(path.c_str(), Model3D);
	nmeshes = Model3D.size();

	for (int i = 0; i < nmeshes; i++)
	{
		crea_VAO_Vector_MeshObj(&Model3D[i]);
		Model3D[i].ModelM = mat4(1.0);
		Model3D[i].ModelM = translate(Model3D[i].ModelM, vec3(2.0, -3.7, -1.2));
		Model3D[i].ModelM = scale(Model3D[i].ModelM, vec3(3.0, 3.0, 3.0));
		Model3D[i].ModelM = rotate(Model3D[i].ModelM, radians(90.0f), vec3(0.0, 1.0, 0.0));
		Model3D[i].nome = "Cottage";
		Model3D[i].sceltaVS = 3;
		Model3D[i].sceltaFS = 6;
	}
	ScenaObj.push_back(Model3D);
	Model3D.clear();

	// Casette
	name = "Cartoon_House.obj";
	path = meshDir + name;
	obj = loadAssImp(path.c_str(), Model3D);
	nmeshes = Model3D.size();
	for (int repX = 0; repX < 4; repX++) 
	{
		for (int repZ = 0; repZ < 4; repZ++)
		{
			for (int i = 0; i < nmeshes; i++)
			{
				crea_VAO_Vector_MeshObj(&Model3D[i]);
				Model3D[i].ModelM = mat4(1.0);
				Model3D[i].ModelM = translate(Model3D[i].ModelM, vec3(18.0 + (repX * 7.0), -3.6, 19.0 - (repZ * 7.0)));
				Model3D[i].ModelM = scale(Model3D[i].ModelM, vec3(0.004, 0.004, 0.004));
				Model3D[i].ModelM = rotate(Model3D[i].ModelM, radians(180.0f), vec3(0.0, 1.0, 0.0));
				Model3D[i].nome = "Casina";
				Model3D[i].sceltaVS = 3;
				Model3D[i].sceltaFS = 6;
			}

			ScenaObj.push_back(Model3D);
		}
	}

	for (int repX = 0; repX < 4; repX++)
	{
		for (int repZ = 0; repZ < 4; repZ++)
		{
			for (int i = 0; i < nmeshes; i++)
			{
				crea_VAO_Vector_MeshObj(&Model3D[i]);
				Model3D[i].ModelM = mat4(1.0);
				Model3D[i].ModelM = translate(Model3D[i].ModelM, vec3(-39.0 + (repX * 7.0), -3.6, 19.0 - (repZ * 7.0)));
				Model3D[i].ModelM = scale(Model3D[i].ModelM, vec3(0.004, 0.004, 0.004));
				Model3D[i].ModelM = rotate(Model3D[i].ModelM, radians(180.0f), vec3(0.0, 1.0, 0.0));
				Model3D[i].nome = "Casina";
				Model3D[i].sceltaVS = 3;
				Model3D[i].sceltaFS = 6;
			}

			ScenaObj.push_back(Model3D);
		}
	}

	Model3D.clear();
}

void INIT_CAMERA_PROJECTION(void)
{
	// Imposto la telecamera
	ViewSetup = {};
	ViewSetup.position = vec4(0.0, 0.5, 70.0, 0.0);
	ViewSetup.target = vec4(0.0, 0.0, 0.0, 0.0);
	ViewSetup.direction = ViewSetup.target - ViewSetup.position;
	ViewSetup.upVector = vec4(0.0, 1.0, 0.0, 0.0);

	// Imposto la proiezione prospettica
	PerspectiveSetup = {};
	PerspectiveSetup.aspect = (GLfloat)width / (GLfloat)height;
	PerspectiveSetup.fovY = 45.0f;
	PerspectiveSetup.far_plane = 2000.0f;
	PerspectiveSetup.near_plane = 0.1f;
}

//int counter = 0;
void drawScene(void)
{
	//printf("%d ", counter++);
	glUniformMatrix4fv(MatrixProj, 1, GL_FALSE, value_ptr(Projection));
	View = lookAt(vec3(ViewSetup.position), vec3(ViewSetup.target), vec3(ViewSetup.upVector));
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Disegno Sky box
	glDepthMask(GL_FALSE);
	glUseProgram(programId1);
	glUniform1i(glGetUniformLocation(programId1, "skybox"), 0);
	glUniformMatrix4fv(MatrixProjS, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatViewS, 1, GL_FALSE, value_ptr(View));
	glBindVertexArray(Scena[0].VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, Scena[0].indici.size() * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);

	// Costruisco la matrice di Vista che applicata ai vertici in coordinate del mondo li trasforma nel sistema di riferimento della camera.
	// Passo al Vertex Shader il puntatore alla matrice View, che sara' associata alla variabile Uniform mat4 Projection
	// all'interno del Vertex shader. Uso l'identificatio MatView
	glPointSize(10.0);

	// Il cappello è riflettente
	glUseProgram(programIdr);
	glUniformMatrix4fv(MatrixProjR, 1, GL_FALSE, value_ptr(Projection));
	glUniformMatrix4fv(MatModelR, 1, GL_FALSE, value_ptr(Scena[1].ModelM));
	glUniformMatrix4fv(MatViewR, 1, GL_FALSE, value_ptr(View));
	glUniform3f(loc_view_posR, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);
	glBindVertexArray(Scena[1].VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawElements(GL_TRIANGLES, (Scena[1].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
	
	// Cambio program per renderizzare tutto il resto della scena
	glUseProgram(programId);
	glUniformMatrix4fv(MatView, 1, GL_FALSE, value_ptr(View));

	// Passo allo shader il puntatore a  colore luce, posizione ed intensit�
	glUniform3f(light_unif.light_position_pointer, light.position.x + 10 * cos(radians(angolo)), light.position.y, light.position.z + 10 * sin(radians(angolo)));
	glUniform3f(light_unif.light_position_pointer, light.position.x, light.position.y, light.position.z);
	glUniform3f(light_unif.light_color_pointer, light.color.r, light.color.g, light.color.b);
	glUniform1f(light_unif.light_power_pointer, light.power);

	// Passo allo shader il puntatore alla posizione della camera
	glUniform3f(loc_view_pos, ViewSetup.position.x, ViewSetup.position.y, ViewSetup.position.z);

	for (int k = 1; k < Scena.size(); k++)
	{
		// Trasformazione delle coordinate dell'ancora dal sistema di riferimento dell'oggetto in sistema
		// di riferimento del mondo premoltiplicando per la matrice di Modellazione.
		Scena[k].ancora_world = Scena[k].ancora_obj;
		Scena[k].ancora_world = Scena[k].ModelM * Scena[k].ancora_world;

		// Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, 
		// che sara' associata alla variabile Uniform mat4 Projection
		// all'interno del Vertex shader. Uso l'identificatio MatModel
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].ModelM));
		glUniform1i(lsceltaVS, Scena[k].sceltaVS);
		glUniform1i(lsceltaFS, Scena[k].sceltaFS);
		// Passo allo shader il puntatore ai materiali
		glUniform3fv(light_unif.material_ambient, 1, value_ptr(materials[Scena[k].material].ambient));
		glUniform3fv(light_unif.material_diffuse, 1, value_ptr(materials[Scena[k].material].diffuse));
		glUniform3fv(light_unif.material_specular, 1, value_ptr(materials[Scena[k].material].specular));
		glUniform1f(light_unif.material_shininess, materials[Scena[k].material].shininess);
		glBindVertexArray(Scena[k].VAO);

		if (visualizzaAncora == TRUE)
		{
			// Visualizzo l'ancora dell'oggetto
			int ind = Scena[k].indici.size() - 1;
			glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, BUFFER_OFFSET(ind * sizeof(GLuint)));
		} 

		if (k < 5 && k > 6)
		{
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, textureNull);
		}
		else if (k == 5 || k == 6)
		{
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, textureBraccia);
		}
		else if (k < Scena.size() - 1)
		{
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, textureSuolo);
		}
		else
		{
			glUniform1i(loc_texture, 0);
			glBindTexture(GL_TEXTURE_2D, textureMakima);
		}

		glDrawElements(GL_TRIANGLES, (Scena[k].indici.size() - 1) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Rotazione braccia
		if (k == 5 || k == 6)
		{
			Scena[k].ModelM = rotate(Scena[k].ModelM, radians(angolo), vec3(0.0, 1.0, 0.0));
		}
	}
	// Visualizzo gli oggetti di tipo Mesh Obj caricati dall'esterno:
	// la j-esima Mesh e' costituita da ScenaObj[j].size() mesh.
	for (int j = 0; j < ScenaObj.size(); j++)
	{
		for (int k = 0; k < ScenaObj[j].size(); k++)
		{	
			// Passo al Vertex Shader il puntatore alla matrice Model dell'oggetto k-esimo della Scena, che sara' associata alla variabile Uniform mat4 Projection
			// all'interno del Vertex shader. Uso l'identificatio MatModel
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(ScenaObj[j][k].ModelM));
			glUniform1i(lsceltaVS, ScenaObj[j][k].sceltaVS);
			glUniform1i(lsceltaFS, ScenaObj[j][k].sceltaFS);
			// Passo allo shader il puntatore ai materiali
			glUniform3fv(light_unif.material_ambient, 1, value_ptr(ScenaObj[j][k].materiale.ambient));
			glUniform3fv(light_unif.material_diffuse, 1, value_ptr(ScenaObj[j][k].materiale.diffuse));
			glUniform3fv(light_unif.material_specular, 1, value_ptr(ScenaObj[j][k].materiale.specular));
			glUniform1f(light_unif.material_shininess, ScenaObj[j][k].materiale.shininess);
			glBindVertexArray(ScenaObj[j][k].VAO);
			glDrawElements(GL_TRIANGLES, (ScenaObj[j][k].indici.size()) * sizeof(GLuint), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}

	glutSwapBuffers();
}

void update(int a)
{
	angolo += 0.05;

	if (angolo >= 0 && angolo < 90.0)
	{
		angolo += 0.005;
	}
	else
	{
		angolo = 0;
	}

	glutPostRedisplay();
	glutTimerFunc(10, update, 0);
}

// Window resizing
void resize(int w, int h)
{
	// Imposto la matrice di proiezione per la scena da diegnare
	Projection = perspective(radians(PerspectiveSetup.fovY), PerspectiveSetup.aspect, PerspectiveSetup.near_plane, PerspectiveSetup.far_plane);

	// Rapporto larghezza altezza di tutto cio' che e' nel mondo
	float AspectRatio_mondo = (float)(width) / (float)(height);

	// Se l'aspect ratio del mondo e' diversa da quella della finestra 
	if (AspectRatio_mondo > w / h)
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_up = (float)w;
		h_up = w / AspectRatio_mondo;
	}
	else
	{
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_up = h * AspectRatio_mondo;
		h_up = (float)h;
	}
}

// Keyboard movement
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

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// Inizializzo finestra per il rendering della scena 3d 
	// con tutti i suoi eventi le sue inizializzazioni e le sue impostazioni
	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Scena 3D");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);

	//gestione input utente
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
	glutMotionFunc(mouseActiveMotion); 

	// Uncomment here if needed
	//glutPassiveMotionFunc(my_passive_mouse);
	
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

	// Abilita l'uso del Buffer di Profondit� per la gestione dell'eliminazione dlele superifici nascoste
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
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

	lsceltaVS = glGetUniformLocation(programId, "sceltaVS");
	lsceltaFS = glGetUniformLocation(programId, "sceltaFS");
	loc_view_pos = glGetUniformLocation(programId, "ViewPos");
	loc_texture = glGetUniformLocation(programId, "id_tex");
	// Location delle variabili uniformi per la gestione della luce
	light_unif.light_position_pointer = glGetUniformLocation(programId, "light.position");
	light_unif.light_color_pointer = glGetUniformLocation(programId, "light.color");
	light_unif.light_power_pointer = glGetUniformLocation(programId, "light.power");

	// Location delle variabili uniformi per la gestione dei materiali
	light_unif.material_ambient = glGetUniformLocation(programId, "material.ambient");
	light_unif.material_diffuse = glGetUniformLocation(programId, "material.diffuse");
	light_unif.material_specular = glGetUniformLocation(programId, "material.specular");
	light_unif.material_shininess = glGetUniformLocation(programId, "material.shininess");

	// location variabili uniformi per lo shader della gestione della cubemap
	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model 
	// (in vertex shader)
	// Questo identificativo sar� poi utilizzato per il trasferimento della matrice Model 
	// al Vertex Shader
	MatrixProjS = glGetUniformLocation(programId1, "Projection");
	MatViewS = glGetUniformLocation(programId1, "View");

	// Chiedo che mi venga restituito l'identificativo della variabile uniform mat4 Model 
	// (in vertex shader)
	// QUesto identificativo sar� poi utilizzato per il trasferimento della matrice Model 
	//al Vertex Shader
	MatModelR = glGetUniformLocation(programIdr, "Model");
	MatViewR = glGetUniformLocation(programIdr, "View");
	MatrixProjR = glGetUniformLocation(programIdr, "Projection");
	loc_view_posR = glGetUniformLocation(programIdr, "ViewPos");
	loc_cubemapR = glGetUniformLocation(programIdr, "cubemap");

	PlaySound(TEXT("sigla.wav"), NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);

	glutMainLoop();
}
