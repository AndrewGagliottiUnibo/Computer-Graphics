#pragma once
#include "Lib.h"

class Shape
{

public:
	GLuint VAO;
	GLuint VBO_Geometry;
	GLuint VBO_Colors;
	int nTriangles;

	// Vertices
	vector<vec3> vertices;

	// Curve vertices
	vector<vec3> controlPoints;
	vector<vec4> colors;

	// Number of vertices
	int nv;

	// Object position
	float posx;
	float posy;

	// Object scale
	float scalex;
	float scaley;

	// Collision and bounding box attributes
	vec4 corner_b_obj;
	vec4 corner_t_obj;
	vec4 corner_b;
	vec4 corner_t;

	// Model matrix: T * R * S
	mat4 Model;
	int choiceVS;
	int choiceFS;

protected:
private:
};
