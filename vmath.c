#include"include/vmath.h"

float dot_product(float *a, float *b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void normalize(float *v) {
	float mag = sqrtf(powf(v[0], 2) + powf(v[1], 2) + powf(v[2], 2));
	
	v[0] /= mag;
	v[1] /= mag;
	v[2] /= mag;
}

void find_norm(struct Matrix *m, int p1, int p2, int p3,
		float *norm_out) {
	//create vectors
	float tempA[] = {
		m->m[0][p2] - m->m[0][p1],
		m->m[1][p2] - m->m[1][p1],
		m->m[2][p2] - m->m[2][p1],
	};
	
	float tempB[] = {
		m->m[0][p3] - m->m[0][p1],
		m->m[1][p3] - m->m[1][p1],
		m->m[2][p3] - m->m[2][p1]
	};
	
	//cross product
	norm_out[0] = tempA[1]*tempB[2] - tempA[2]*tempB[1];
	norm_out[1] = tempA[2]*tempB[0] - tempA[0]*tempB[2];
	norm_out[2] = tempA[0]*tempB[1] - tempA[1]*tempB[0];
}

