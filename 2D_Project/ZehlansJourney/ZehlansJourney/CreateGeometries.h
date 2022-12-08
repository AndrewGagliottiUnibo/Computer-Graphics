#pragma once
#include <iostream>
#include "Shape.h"
#include "Hermitte.h"
#include "Lib.h"

/*
	VAO generators.
*/
void create_VAO_Vector(Shape *shape);

/*
	buildShip: builds the background.
*/
void buildBackground(Shape *background);

/*
	buildShip: builds the motherShip in the center of the screen.
*/
void buildShip(Shape *ship, vec4 colorTop, vec4 colorBottom);

/*
	buildBlob: builds a blob.
*/
void buildBlob(Shape *blob, vec4 colorIn, vec4 colorOut);

/*
	buildBullet: position a bullet in the ship gun.
*/
void buildBullet(Shape *bullet, vec4 color);

/*
	createStar: builds a basic star for background.
*/
void buildStars(Shape *star, vec4 colorCenter, vec4 colorEdges);
