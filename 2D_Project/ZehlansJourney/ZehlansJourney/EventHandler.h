#pragma once
#include <iostream>
#include "CreateGeometries.h"
#include "Elements.h"
#include "Lib.h"

/*
	update: everything on screen is updated.
*/
void update(int value);

/*
	keyboardPressedEvent: read a key input and calls others funcs.
*/
void keyboardPressedEvent(unsigned char key, int x, int y);

/*
	mouse: shows hitboxes
*/
void mouse(int button, int state, int x, int y);

/*
	resize: resize geometries depending on the screen.
*/
void resize(GLsizei w, GLsizei h);

/*
	updateShip: update ship position.
*/
void updateShip(unsigned char key);

/*
	updateBlob: update Blob status if it is shot.
*/
void updateBlob(int value);

/*
	shoot: shots the bullet.
*/
void shoot(int value);

/*
	checkCollision: checks collisions between figures.
*/
bool checkCollision(Shape *blob, Shape *bullet);

/*
	scoreUp: adds a point.
*/
void scoreUp(int value);

/*
	getScore: returns value of score.
*/
int getScore();

/*
	endGame: kills application.
*/
void endGame();