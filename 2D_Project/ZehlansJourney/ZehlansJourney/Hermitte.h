#pragma once
#include "Lib.h"
#include "Shape.h"

// Math functions
#define PHI0(t) (2.0 * t * t * t - 3.0 * t * t + 1)
#define PHI1(t) (t * t * t - 2.0 * t * t + t)
#define PSI0(t) (-2.0 * t * t * t + 3.0 * t * t)
#define PSI1(t) (t * t * t - t * t)

// Math functions used for Hermitte interpolation
float dx(int i, float *t, float Tens, float Bias, float Cont, Shape *Fig);

float dy(int i, float *t, float Tens, float Bias, float Cont, Shape *Fig);

void InterpolazioneHermitte(float *t, Shape *Fig, vec4 color_top, vec4 color_bot);
