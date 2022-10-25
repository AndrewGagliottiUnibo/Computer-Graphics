#pragma once
#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"

/*
	update: everything on screen is updated.
*/
void update(const int value);

/*
	updateShip: update ship position.
*/
void updateShip(const unsigned char key, const int x, const int y);

/*
	updateBlob: update Blob status if it is shot
*/
void updateBlob();

/*
	scoreUp: adds a point.
*/
void scoreUp();

/*
	scoreDown: removes 3 points.
*/
void scoreDown();

/*
	endGame: kills application.
*/
void endGame();