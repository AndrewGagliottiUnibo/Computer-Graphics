#pragma once
#pragma once
#include "Lib.h"
#include <string>

// ViewSetup
struct 
{
	vec4 position;
	vec4 target;
	vec4 upVector;
	vec4 direction;
} ViewSetup;

// PerspectiveSetup
struct 
{
	float fovY; 
	float aspect;
	float near_plane;
	float far_plane;
} PerspectiveSetup;

struct Character 
{
	unsigned int TextureID; // ID handle of the glyph texture
	ivec2 Size;      		// Size of glyph
	ivec2 Bearing;   		// Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

// Shader name and value
typedef struct 
{
	string name;
	int value;
} Shader;

// Light of the scene
typedef struct 
{
	vec3 position;
	vec3 color;
	GLfloat power;
} point_light;

// Shader illumination
typedef struct 
{
	GLuint light_position_pointer;
	GLuint light_color_pointer;
	GLuint light_power_pointer;
	GLuint material_diffuse;
	GLuint material_ambient;
	GLuint material_specular;
	GLuint material_shininess;
} LightShaderUniform;

// Materials
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

// Shader options for the menu
typedef enum 
{
	NONE,
	GOURAD_SHADING,
	PHONG_SHADING,
	ONDE_SHADING,
	BANDIERA_SHADING,
} ShaderOption;

// Material descriptions
typedef struct 
{
	string name;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	GLfloat shininess;
} Material;

// Shading type
typedef enum 
{
	GOURAUD,
	PHONG,
	BLINN,
	TOON,
	PASS_THROUGH,
	WAVE
} ShadingType;

// Mesh data
typedef struct 
{
	vector<vec3> vertici;
	vector<vec4> colori;
	vector <GLuint> indici;
	vector<vec3> normali;
	vector<vec2> texCoords;
	mat4 ModelM;
	int sceltaVS;
	int sceltaFS;
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	GLuint VBO_normali;
	GLuint EBO_indici;
	GLuint VBO_coord_texture;
	ShadingType shading;
	// Copio qui i dati del materiale
	MaterialType material;
	Material materiale;
	string nome;
	vec4 ancora_obj;
	vec4 ancora_world;
} Mesh;

static vector<Mesh> Scena;
