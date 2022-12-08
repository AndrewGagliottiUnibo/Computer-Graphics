#include "CreateGeometries.h"
#include "Values.h"
#define PI 3.14159265358979323846

void create_VAO_Vector(Shape *shape)
{
	glGenVertexArrays(1, &shape->VAO);
	glBindVertexArray(shape->VAO);
	// Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &shape->VBO_Geometry);
	glBindBuffer(GL_ARRAY_BUFFER, shape->VBO_Geometry);
	glBufferData(GL_ARRAY_BUFFER, shape->vertices.size() * sizeof(vec3), shape->vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(0);

	// Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &shape->VBO_Colors);
	glBindBuffer(GL_ARRAY_BUFFER, shape->VBO_Colors);
	glBufferData(GL_ARRAY_BUFFER, shape->colors.size() * sizeof(vec4), shape->colors.data(), GL_STATIC_DRAW);
	// Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glEnableVertexAttribArray(1);
}

void buildBackground(Shape *background)
{
	// Inserimento vertici e colori
	background->vertices.push_back(vec3(0.0, 0.0, 0.0));
	background->colors.push_back(cBackground);
	background->vertices.push_back(vec3(0.0, 1.0, 0.0));
	background->colors.push_back(cBackground);
	background->vertices.push_back(vec3(1.0, 0.0, 0.0));
	background->colors.push_back(cBackground);
	background->vertices.push_back(vec3(1.0, 1.0, 0.0));
	background->colors.push_back(cBackground);

	background->nv = background->vertices.size();

	// Costruzione matrice di Modellazione
	background->Model = mat4(1.0);
	background->Model = scale(background->Model, vec3(background->scalex, background->scaley, 1.0));
}

void buildShip(Shape *ship, vec4 colorTop, vec4 colorBottom)
{
	float *t;

	ship->controlPoints.push_back(vec3(0.0, -2.0, 0.0));
	ship->controlPoints.push_back(vec3(-5.0, -5.0, 0.0));
	ship->controlPoints.push_back(vec3(-5.0, 1.5, 0.0));
	ship->controlPoints.push_back(vec3(-4.0, 2.0, 0.0));
	ship->controlPoints.push_back(vec3(-2.5, 4.0, 0.0));
	ship->controlPoints.push_back(vec3(-2.5, 6.0, 0.0));
	ship->controlPoints.push_back(vec3(-2.0, 7.0, 0.0));
	ship->controlPoints.push_back(vec3(0.0, 10.0, 0.0));
	ship->controlPoints.push_back(vec3(2.0, 7.0, 0.0));
	ship->controlPoints.push_back(vec3(2.5, 6.0, 0.0));
	ship->controlPoints.push_back(vec3(2.5, 4.0, 0.0));
	ship->controlPoints.push_back(vec3(4.0, 2.0, 0.0));
	ship->controlPoints.push_back(vec3(5.0, 1.5, 0.0));
	ship->controlPoints.push_back(vec3(5.0, -5.0, 0.0));
	ship->controlPoints.push_back(vec3(0.0, -2.0, 0.0));
	ship->colors.push_back(colorBottom);

	t = new float[ship->controlPoints.size()];
	int c = 0;
	float step = 1.0 / (float)(ship->controlPoints.size() - 1);

	for (int i = 0; i < ship->controlPoints.size(); i++)
	{
		t[i] = (float)c * step;
		c++;
	}

	InterpolazioneHermitte(t, ship, colorTop, colorBottom);
	ship->nv = ship->vertices.size();

	ship->Model = mat4(1.0);
	ship->Model = translate(ship->Model, vec3(ship->posx, ship->posy, 0.0));
	ship->Model = scale(ship->Model, vec3(ship->scalex, ship->scaley, 1.0));
}

void buildBlob(Shape *blob, vec4 colorIn, vec4 colorOut)
{
	int i;
	float stepA = (2 * PI) / blob->nTriangles;
	float t;

	blob->posx = blobDelay + (rand() % (WIDTH - blobDelay));
	blob->posy = (blobBottomHeight + rand() % blobTopPhase);
	blob->vertices.push_back(vec3(0.0, 0.0, 0.0));
	blob->colors.push_back(colorIn);

	for (i = 0; i <= blob->nTriangles + 1; i++)
	{
		t = (float)i * stepA;

		blob->vertices.push_back(vec3(cos(t), sin(t), 0.0));
		blob->colors.push_back(colorIn);
	}
	blob->vertices.push_back(vec3(0.0, 0.0, 0.0));
	blob->colors.push_back(colorIn);
	blob->nv = blob->vertices.size();

	// Alone
	blob->vertices.push_back(vec3(0.0, 0.0, 0.0));
	blob->colors.push_back(colorOut);
	for (i = 0; i <= blob->nTriangles; i++)
	{
		t = (float)i * stepA;

		blob->vertices.push_back(vec3(2.0 * cos(t), 2.0 * sin(t), 0.0));
		blob->colors.push_back(colorOut);
	}

	// Bounding box
	float ymin = { 0.0 };
	float ymax = { 0.0 };
	float xmin = { 0.0 };
	float xmax = { 0.0 };

	for (int i = 0; i < blob->nv; i++)
	{
		if (blob->vertices[i].x > xmax)
		{
			xmax = blob->vertices[i].x;
		}
		if (blob->vertices[i].x < xmin)
		{
			xmin = blob->vertices[i].x;
		}
		if (blob->vertices[i].y > ymax)
		{
			ymax = blob->vertices[i].y;
		}
		if (blob->vertices[i].y < ymin)
		{
			ymin = blob->vertices[i].y;
		}
	}

	// x 2 per l'effetto alone
	blob->corner_t_obj = vec4(2 * xmax, 2 * ymax, 0.0, 1.0);
	blob->corner_b_obj = vec4(2 * xmin, 2 * ymin, 0.0, 1.0);
	blob->vertices.push_back(vec3(2 * xmin, 2 * ymin, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	blob->vertices.push_back(vec3(2 * xmax, 2 * ymin, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	blob->vertices.push_back(vec3(2 * xmax, 2 * ymax, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	blob->vertices.push_back(vec3(2 * xmin, 2 * ymin, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	blob->vertices.push_back(vec3(2 * xmin, 2 * ymax, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	blob->vertices.push_back(vec3(2 * xmax, 2 * ymax, 0.0));
	blob->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));

	blob->nv = blob->vertices.size();

	// Costruzione matrice di Modellazione
	blob->Model = mat4(1.0);
	blob->Model = translate(blob->Model, vec3(blob->posx, blob->posy, 0.0));
	blob->Model = scale(blob->Model, vec3(blob->scalex, blob->scaley, 1.0));
}

void buildBullet(Shape *bullet, vec4 color)
{
	bullet->vertices.push_back(vec3(0.0, 0.0, 0.0));
	bullet->colors.push_back(color);
	bullet->vertices.push_back(vec3(0.3, 0.0, 0.0));
	bullet->colors.push_back(color);
	bullet->vertices.push_back(vec3(0.3, 0.6, 0.0));
	bullet->colors.push_back(color);
	bullet->vertices.push_back(vec3(0.0, 0.6, 0.0));
	bullet->colors.push_back(color);
	bullet->vertices.push_back(vec3(0.0, 0.0, 0.0));
	bullet->colors.push_back(color);

	// Bounding box
	float ymin = { 0.0 };
	float ymax = { 0.0 };
	float xmin = { 0.0 };
	float xmax = { 0.0 };

	for (int i = 0; i < bullet->nv; i++)
	{
		if (bullet->vertices[i].x > xmax)
		{
			xmax = bullet->vertices[i].x;
		}
		if (bullet->vertices[i].x < xmin)
		{
			xmin = bullet->vertices[i].x;
		}
		if (bullet->vertices[i].y > ymax)
		{
			ymax = bullet->vertices[i].y;
		}
		if (bullet->vertices[i].y < ymin)
		{
			ymin = bullet->vertices[i].y;
		}
	}
	bullet->corner_t_obj = vec4(xmax, ymax, 0.0, 1.0);
	bullet->corner_b_obj = vec4(xmin, ymin, 0.0, 1.0);
	bullet->nv = bullet->vertices.size();

	// Costruzione matrice di modellazione
	bullet->Model = mat4(1.0);
	bullet->Model = translate(bullet->Model, vec3(bullet->posx, bullet->posy, 0.0));
	bullet->Model = scale(bullet->Model, vec3(bullet->scalex, bullet->scaley, 1.0));
}

void buildStars(Shape *star, vec4 colorCenter, vec4 colorEdges)
{
	// Background Stars
	float stepA = (2 * PI) / star->nTriangles;
	float t;

	star->vertices.push_back(vec3(0.0, 0.0, 0.0));
	star->colors.push_back(colorCenter);

	for (int i = 0; i <= star->nTriangles; i++)
	{
		t = (float)i * stepA;

		if (i % 2 == 0)
		{
			star->vertices.push_back(vec3(cos(t), sin(t), 0.0));
		}
		else
		{
			star->vertices.push_back(vec3(0.5 * cos(t), 0.5 * sin(t), 0.0));
		}

		// Colore
		star->colors.push_back(colorEdges);
	}

	star->nv = star->vertices.size();

	// Costruzione matrice di modellazione
	int valueXT = rand() % WIDTH;
	int valueYT = rand() % HEIGHT;
	int valueS = rand() % starScale;
	star->Model = mat4(1.0);
	star->Model = translate(star->Model, vec3(valueXT, valueYT, 0.0));
	star->Model = scale(star->Model, vec3(valueS, valueS, 1.0));
}
