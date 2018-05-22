/* draw.h
 * 
 * Plotting pixels onto a frame
 * */

#ifndef DRAW_H
#define DRAW_H

#include<stdio.h>
#include<stdlib.h>
#include<float.h>

#include"dimen.h"
#include"matrix.h"
#include"vmath.h"
#include"lighting.h"

struct Light;

struct Pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

typedef struct Pixel Frame[IMG_HEIGHT][IMG_WIDTH];

typedef float zbuffer[IMG_HEIGHT][IMG_WIDTH];

//writes a pixel value to a spot on the grid
//(0, 0) is the bottom left of the grid
void plot_point(Frame grid, zbuffer b,
		int x, int y, float z, struct Pixel *p);

//pixel is for custom colors
void draw_line(Frame grid, zbuffer buf, struct Pixel *p,
		int x1, int y1, float z1,
		int x2, int y2, float z2);

//p is the color, assumes length of m is even
void draw_lines(Frame grid, zbuffer b, struct Matrix *m, struct Pixel *p);

void pixel_color(struct Pixel *p, unsigned char r, unsigned char g, unsigned char b);

//helper function to find the norm of two vectors
//p1, p2, p3 are indices
//goes counterclockwise starting at p1
void find_norm(struct Matrix *m, int p1, int p2, int p3,
		float *norm_out);

void draw_polygons(Frame f, zbuffer buf, struct Matrix *m, struct Pixel *p,
		struct Light *l, float *view_vect);

void render_scanlines(Frame f, zbuffer b, struct Matrix *m, struct Pixel *p,
		int p1, int p2, int p3);

//sets Frame to all 0, zbuffer to -FLT_MAX
void clear(Frame f, zbuffer b);

#endif

