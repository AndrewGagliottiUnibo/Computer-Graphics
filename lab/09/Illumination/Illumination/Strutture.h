#pragma once
#include "Lib.h"
#include <string>

struct
{
	vec4 position;
	vec4 target;
	vec4 upVector;
	vec4 direction;
} ViewSetup;

struct
{
	float fovY, aspect, near_plane, far_plane;
} PerspectiveSetup;

struct Character
{
	unsigned int TextureID; // ID handle of the glyph texture
	glm::ivec2 Size;		// Size of glyph
	glm::ivec2 Bearing;		// Offset from baseline to left/top of glyph
	unsigned int Advance;	// Horizontal offset to advance to next glyph
};

typedef struct
{
	string name;
	int value;
} Shader;

typedef struct
{
	glm::vec3 position;
	glm::vec3 color;
	GLfloat power;
} point_light;

// Variabili uniformi per la grandezza delle luci: alcune di queste vengono al
// di fuori della applicazione
typedef struct
{
	GLuint light_position_pointer;
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	// Esponente del coseno per gestire l'illuminazione: pi� � grande
	// pi� stretto � il coseno e pi� � sottile il cono di riflessione della luce.
	GLuint material_shininess;
} LightShaderUniform;

typedef enum
{
	RED_PLASTIC,
	EMERALD,
	BRASS,
	SNOW_WHITE,
	YELLOW,
	ROSA,
	MARRONE,
	NO_MATERIAL
} MaterialType;

typedef enum
{
	NONE,
	GOURAD_SHADING,
	PHONG_SHADING,
	ONDE_SHADING,
	BANDIERA_SHADING,
} ShaderOption;

typedef struct
{
	string name;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
} Material;

typedef enum
{
	GOURAUD,
	PHONG,
	BLINN,
	TOON,
	PASS_THROUGH,
	WAVE
} ShadingType;

typedef struct
{
	vector<vec3> vertici;
	vector<vec4> colori;
	vector<GLuint> indici;
	vector<vec3> normali;
	vector<vec2> texCoords;
	mat4 Model;
	int sceltaVS;
	int sceltaFS;
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint VBO_normali;
	GLuint EBO_indici;
	GLuint VBO_coord_texture;
	ShadingType shading;
	// Per specificare il comportamento di un oggetto quando viene
	// sottoposto ad illuminazione. Questa pu� essere diffusiva
	// oppure ambientale. Con la luce ambientale si definisce come
	// l'oggetto si comporta quando non esposto a luce diffusiva.
	MaterialType material;
	string nome;
	vec4 ancora_obj;
	vec4 ancora_world;
} Mesh;

static vector<Mesh> Scena;
