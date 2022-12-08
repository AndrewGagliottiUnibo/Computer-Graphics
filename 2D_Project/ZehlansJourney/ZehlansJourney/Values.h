#pragma once
#include "Lib.h"

// MotherShip
static int shipPositionX = 2;
static int shipPositionY = 8;
static vec4 cTopShip = {0.5, 0.2, 1.0, 1.0};
static vec4 cBotShip = {0.5, 1.0, 0.5, 1.0};
static float shipScale = 6.0;

// Blob
static vec4 cBlobIn = {0.3, 1.0, 0.3, 1.0};
static vec4 cBlobOut = {1.0, 1.0, 0.0, 1.0};
static float blobScale = 20.0;
static int blobBottomHeight = 600;
static int blobTopPhase = 80;
static int blobDelay = 50;
static int blobTriangles = 180;

// Bullet
static vec4 cBullet = {1.0, 0.2, 1.0, 1.0};
static float bulletScale = 10.0;
static int bulletXDelay = -1.0;
static int bulletYDelay = 52;

// Star
static vec4 cStarsCenter = {1.0, 1.0, 1.0, 1.0};
static vec4 cStarsEdges = {0.6, 0.5, 0.0, 1.0};
static int starScale = 7;
static int starsTriangles = 10;
static int maxStars = 100;

// Background
static vec4 cBackground = {0.3, 0.3, 0.3, 0.5};

// Others
static float DEFAULT = 0.0;
static int movementDelay = 5;
