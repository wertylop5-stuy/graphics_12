/* matrix.h
 *
 * Creation of matrices and interactions with the matrix
 * */

#ifndef MATRIX_H
#define MATRIX_H

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

struct Matrix {
	float **m;
	int rows;
	int cols;
	int back;	//last-most col that is being used
};

struct Matrix* new_matrix(int r, int c);

void print_matrix(struct Matrix *m);

//turns m into an indentity matrix
//must be a square matrix
void ident(struct Matrix *m);

//b will be modified with the result
//cols dimensions of a must = rows of b
void matrix_mult(struct Matrix const *a, struct Matrix *b);

void free_matrix(struct Matrix *m);

struct Matrix* copy_matrix(struct Matrix *m);

//doubles the amount of columns
void resize(struct Matrix *m);

void push_point(struct Matrix *m, float x, float y, float z);

void push_edge(struct Matrix *m, float x1, float y1,
		float z1, float x2, float y2, float z2);

void push_polygon(struct Matrix *m,
	float x0, float y0, float z0,
	float x1, float y1, float z1,
	float x2, float y2, float z2
);

#endif
