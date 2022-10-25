#pragma once
#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"

/*
	buildShip: builds the motherShip in the center of the screen.
*/
void buildShip(vec4 colorTop, vec4 colorBottom, Shape* shape);

/*
	buildBlob: builds a blob.
*/
void buildBlob(vec4 colorTop, vec4 colorBottom, Shape* shape);

/*
	positionBlob: position a blob in a random position.
*/
void positionBlob(Shape* shape);

/*
	buildBullet: position a bullet in the ship gun.
*/
void buildBullet(Shape* shape);

/*
	resetBullet: reset bullet position.
*/
void resetBullet();

/*
	createStar: builds a basic star for background.
*/
void createStar(Shape* shape);