/* shapes.h
 *  
 * Used to calculate coordinates necessary for drawing
 * certain shapes
 * */

#ifndef SHAPES_H
#define SHAPES_H

#include"matrix.h"

//a,b,c,d are coefficients
#define CUBIC(a, b, c, d, x)	(a*(x)*(x)*(x) + b*(x)*(x) + c*(x) + d)

//used for drawing curves
#define STEP	.01f

//returns transformation matrices
struct Matrix* scale(float a, float b, float c);
struct Matrix* move(float a, float b, float c);
struct Matrix* rotate(int axis, float deg);	//0: x, 1: y, 2: z

//runs generic parametric function at^3 + bt^2 + ct + d with t [0, 1]
//for the x and y coords
void parametric_exec(struct Matrix *m,
		float ax, float bx, float cx, float dx,
		float ay, float by, float cy, float dy);

//take in degrees for how much of the circle to draw
void make_circle(struct Matrix *m, float cx, float cy, float cz, float r, float deg);
void make_hermite(struct Matrix *m, float x0, float y0, float x1, float y1,
	float rx0, float ry0, float rx1, float ry1);
void make_bezier(struct Matrix *m, float x0, float y0, float x1, float y1,
	float x2, float y2, float x3, float y3);

void add_cube(struct Matrix *m, float x, float y, float z,
	float width, float height, float depth);

void add_sphere(struct Matrix *m, float cx, float cy, float cz, float r, int step);
struct Matrix* sphere_points(float cx, float cy, float cz, float r, int step);

//r1 is the circle radius, r2 is the distance from torus center to circle center
void add_torus(struct Matrix *m, float cx, float cy, float cz,
	float r1, float r2, int step);
struct Matrix* torus_points(float cx, float cy, float cz,
	float r1, float r2, int step);

#endif

