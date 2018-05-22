#include"include/shapes.h"

struct Matrix* scale(float a, float b, float c) {
	struct Matrix *temp = new_matrix(4, 4);
	ident(temp);
	
	temp->m[0][0] = a;
	temp->m[1][1] = b;
	temp->m[2][2] = c;
	
	return temp;
}

struct Matrix* move(float a, float b, float c) {
	struct Matrix *temp = new_matrix(4, 4);
	ident(temp);
	
	temp->m[0][3] = a;
	temp->m[1][3] = b;
	temp->m[2][3] = c;
	
	return temp;
}

struct Matrix* rotate(int axis, float deg) {
	struct Matrix *temp = new_matrix(4, 4);
	ident(temp);
	float rad = deg * (M_PI/180.0f);
	
	switch(axis) {
		case 2:
			temp->m[0][0] = cosf(rad);
			temp->m[0][1] = -sinf(rad);
			temp->m[1][0] = sinf(rad);
			temp->m[1][1] = cosf(rad);
		break;
		
		case 0:
			temp->m[1][1] = cosf(rad);
			temp->m[1][2] = -sinf(rad);
			temp->m[2][1] = sinf(rad);
			temp->m[2][2] = cosf(rad);
		break;
		
		case 1:
			temp->m[0][0] = cosf(rad);
			temp->m[2][0] = -sinf(rad);
			temp->m[0][2] = sinf(rad);
			temp->m[2][2] = cosf(rad);
		break;
	}

	return temp;
}

//the p0, p1, etc is just the x or y coord
void parametric_exec(struct Matrix *m,
		float ax, float bx, float cx, float dx,
		float ay, float by, float cy, float dy) {
	float t;
	for (t = 0; t <= 1; t += STEP) {
		push_edge(m,
				CUBIC(ax, bx, cx, dx, t),
				CUBIC(ay, by, cy, dy, t),
				0,
				CUBIC(ax, bx, cx, dx, t+STEP),
				CUBIC(ay, by, cy, dy, t+STEP),
				0);
	}
}

void make_circle(struct Matrix *m, float cx, float cy, float cz,
		float r, float deg) {
	float t;
	for (t = 0; t <= 1; t += STEP) {
		push_edge(m,
				r*cosf(deg*t) + cx,
				r*sinf(deg*t) + cy,
				cz,
				r*cosf(deg*(t+STEP)) + cx,
				r*sinf(deg*(t+STEP)) + cy,
				cz);
	}
}

void make_hermite(struct Matrix *m, float x0, float y0, float x1, float y1,
		float rx0, float ry0, float rx1, float ry1) {
	struct Matrix *h = new_matrix(4, 4);
	
	float temp[4][4] = {
		{2, -2, 1, 1},
		{-3, 3, -2, -1},
		{0, 0, 1, 0},
		{1, 0, 0, 0}
	};
	
	int x;
	for (x = 0; x < 4; x++) {
		memcpy(h->m[x], temp[x], sizeof(temp[x]));
	}
	h->back = 4;
	
	struct Matrix *xpoints = new_matrix(4, 1);
	struct Matrix *ypoints = new_matrix(4, 1);
	
	xpoints->m[0][0] = x0;
	xpoints->m[1][0] = x1;
	xpoints->m[2][0] = rx0;
	xpoints->m[3][0] = rx1;
	xpoints->back = 1;

	ypoints->m[0][0] = y0;
	ypoints->m[1][0] = y1;
	ypoints->m[2][0] = ry0;
	ypoints->m[3][0] = ry1;
	ypoints->back = 1;

	matrix_mult(h, xpoints);
	matrix_mult(h, ypoints);
	
	parametric_exec(m,
		xpoints->m[0][0],xpoints->m[1][0],
		xpoints->m[2][0],xpoints->m[3][0],
		ypoints->m[0][0],ypoints->m[1][0],
		ypoints->m[2][0],ypoints->m[3][0]);
	
	free_matrix(xpoints);
	free_matrix(ypoints);
	free_matrix(h);
}

void make_bezier(struct Matrix *m, float x0, float y0, float x1, float y1,
		float x2, float y2, float x3, float y3) {
	struct Matrix *h = new_matrix(4, 4);
	float temp[4][4] = {
		{-1, 3, -3, 1},
		{3, -6, 3, 0},
		{-3, 3, 0, 0},
		{1, 0, 0, 0}
	};
	int x;
	for (x = 0; x < 4; x++) {
		memcpy(h->m[x], temp[x], sizeof(temp[x]));
	}
	h->back = 4;
	
	struct Matrix *xpoints = new_matrix(4, 1);
	struct Matrix *ypoints = new_matrix(4, 1);
	
	xpoints->m[0][0] = x0;
	xpoints->m[1][0] = x1;
	xpoints->m[2][0] = x2;
	xpoints->m[3][0] = x3;
	xpoints->back = 1;

	ypoints->m[0][0] = y0;
	ypoints->m[1][0] = y1;
	ypoints->m[2][0] = y2;
	ypoints->m[3][0] = y3;
	ypoints->back = 1;

	matrix_mult(h, xpoints);
	matrix_mult(h, ypoints);
	
	parametric_exec(m,
		xpoints->m[0][0],xpoints->m[1][0],
		xpoints->m[2][0],xpoints->m[3][0],
		ypoints->m[0][0],ypoints->m[1][0],
		ypoints->m[2][0],ypoints->m[3][0]);
	
	free_matrix(xpoints);
	free_matrix(ypoints);
	free_matrix(h);
}

void add_cube(struct Matrix *m, float x, float y, float z,
		float width, float height, float depth) {
	float x2 = x + width;
	float y2 = y - height;
	float z2 = z - depth;

	//front
	push_polygon(m,
		x, y, z,
		x, y2, z,
		x2, y, z
	);
	push_polygon(m,
		x2, y, z,
		x, y2, z,
		x2, y2, z
	);
	
	//back (added clockwise?)
	push_polygon(m,
		x2, y2, z2,
		x, y, z2,
		x2, y, z2
	);
	push_polygon(m,
		x, y2, z2,
		x, y, z2,
		x2, y2, z2
	);

	//left side
	push_polygon(m,
		x, y, z2,
		x, y2, z2,
		x, y, z
	);
	push_polygon(m,
		x, y, z,
		x, y2, z2,
		x, y2, z
	);
	
	//right side
	push_polygon(m,
		x2, y, z,
		x2, y2, z,
		x2, y, z2
	);
	push_polygon(m,
		x2, y, z2,
		x2, y2, z,
		x2, y2, z2
	);

	//top
	push_polygon(m,
		x, y, z2,
		x, y, z,
		x2, y, z2
	);
	push_polygon(m,
		x2, y, z2,
		x, y, z,
		x2, y, z
	);

	//bottom (clockwise?)
	push_polygon(m,
		x, y2, z,
		x, y2, z2,
		x2, y2, z
	);
	push_polygon(m,
		x2, y2, z,
		x, y2, z2,
		x2, y2, z2
	);
}

void add_sphere(struct Matrix *m, float cx, float cy, float cz, float r, int step) {
	struct Matrix *res = sphere_points(cx, cy, cz, r, step);
	int step_small = 180/step + 1;
	int x;
	
	for (x = 0/*step_small*7*/; x < res->back/*-(step_small*2)*/; x++) {
		//skip last point
		if (x % step_small == step_small - 1) continue;

		//top pole
		if (x % step_small == 0) {
			
			push_polygon(m,
				res->m[0][x],
				res->m[1][x],
				res->m[2][x],
				res->m[0][(x+1)%res->back],
				res->m[1][(x+1)%res->back],
				res->m[2][(x+1)%res->back],
				res->m[0][(x+1+step_small)%res->back],
				res->m[1][(x+1+step_small)%res->back],
				res->m[2][(x+1+step_small)%res->back]
			);
			
		}
		//bottom pole, trigger the case before the final
		//point in the semicircle (to avoid subtraction)
		else if (x % step_small == step_small - 2) {
			
			push_polygon(m,
				res->m[0][x],
				res->m[1][x],
				res->m[2][x],
				res->m[0][(x+1)%res->back],
				res->m[1][(x+1)%res->back],
				res->m[2][(x+1)%res->back],
				res->m[0][(x+step_small)%res->back],
				res->m[1][(x+step_small)%res->back],
				res->m[2][(x+step_small)%res->back]
			);
			
		}
		else {
			push_polygon(m,
				res->m[0][(x+step_small+1)%res->back],
				res->m[1][(x+step_small+1)%res->back],
				res->m[2][(x+step_small+1)%res->back],
				res->m[0][(x+step_small)%res->back],
				res->m[1][(x+step_small)%res->back],
				res->m[2][(x+step_small)%res->back],
				res->m[0][x],
				res->m[1][x],
				res->m[2][x]
			);
			
			push_polygon(m,
				res->m[0][(x+1)%res->back],
				res->m[1][(x+1)%res->back],
				res->m[2][(x+1)%res->back],
				res->m[0][(x+1+step_small)%res->back],
				res->m[1][(x+1+step_small)%res->back],
				res->m[2][(x+1+step_small)%res->back],
				res->m[0][x],
				res->m[1][x],
				res->m[2][x]
			);
		}
	}
	free_matrix(res);
}

struct Matrix* sphere_points(float cx, float cy, float cz, float r, int step) {
	int t, t1;
	struct Matrix *m = new_matrix(4, 1);
	for (t = 0; t <= 360; t+=step) {
	for (t1 = 0; t1 <= 180; t1+=step) {
		push_point(m,
			r*cosf(t1*(M_PI/180.0f)) + cx,
			r*sinf(t1*(M_PI/180.0f))*cosf(t*(M_PI/180.0f)) + cy,
			r*sinf(t1*(M_PI/180.0f))*sinf(t*(M_PI/180.0f)) + cz
		);
	}
	}
	return m;
}

void add_torus(struct Matrix *m, float cx, float cy, float cz,
		float r1, float r2, int step) {
	struct Matrix *res = torus_points(cx, cy, cz, r1, r2, step);
	int step_big = 360/step;
	int x;
	
	for (x = 0; x < res->back; x++) {
		push_polygon(m,
			res->m[0][x],
			res->m[1][x],
			res->m[2][x],
			res->m[0][(x+step_big)%res->back],
			res->m[1][(x+step_big)%res->back],
			res->m[2][(x+step_big)%res->back],
			res->m[0][(x+step_big+1)%res->back],
			res->m[1][(x+step_big+1)%res->back],
			res->m[2][(x+step_big+1)%res->back]
		);
		push_polygon(m,
			res->m[0][x],
			res->m[1][x],
			res->m[2][x],
			res->m[0][(x+step_big+1)%res->back],
			res->m[1][(x+step_big+1)%res->back],
			res->m[2][(x+step_big+1)%res->back],
			res->m[0][(x+1)%res->back],
			res->m[1][(x+1)%res->back],
			res->m[2][(x+1)%res->back]
		);
	}
	free_matrix(res);
}

struct Matrix* torus_points(float cx, float cy, float cz,
		float r1, float r2, int step) {
	int phi, theta;
	struct Matrix *m = new_matrix(4, 1);
	for (phi = 0; phi <= 360; phi+=step) {
	for (theta = 0; theta <= 360; theta+=step) {
		push_point(m,
			cosf(phi*(M_PI/180.0f))*
				(r1*cosf(theta*(M_PI/180.0f))+r2) + cx,
			r1*sinf(theta*(M_PI/180.0f)) + cy,
			-sinf(phi*(M_PI/180.0f))*
				(r1*cosf(theta*(M_PI/180.0f))+r2) + cz
		);
	}
	}
	return m;
}


