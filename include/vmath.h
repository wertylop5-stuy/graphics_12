/*
 * vmath.h
 *
 * Vector related operations
 * */

#ifndef VMATH_H
#define VMATH_H

#include"matrix.h"

#define SQUARE(x) (x*x)

//all float* are 3 element arrays

float dot_product(float *a, float *b);
void normalize(float *v);

//get the normal of a surface
void find_norm(struct Matrix *m, int p1, int p2, int p3,
		float *norm_out);

#endif
