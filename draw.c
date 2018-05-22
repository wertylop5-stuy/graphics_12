//for qsort
#define _GNU_SOURCE

#include"include/draw.h"

void pixel_color(struct Pixel *p, unsigned char r,
		unsigned char g, unsigned char b) {
	p->r = r;
	p->g = g;
	p->b = b;
}

void plot_point(Frame grid, zbuffer b, int x, int y, float z, struct Pixel *p) {
	//printf("Plotting %d, %d (%d, %d)\n", x, IMG_HEIGHT-1-y, x, y);
	if (x > -1 && y > -1 && b[IMG_HEIGHT-1-y][x] < z) {
		grid[IMG_HEIGHT-1-y][x] = *p;
		b[IMG_HEIGHT-1-y][x] = z;
	}
	//grid[y][x] = *p;	//if top left is (0, 0)
}

void draw_lines(Frame grid, zbuffer b, struct Matrix *m, struct Pixel *p) {
	int x;
	for (x = 1; x < m->back; x+=2) {
		draw_line(grid, b, p,
				m->m[0][x-1], m->m[1][x-1], 0,
				m->m[0][x], m->m[1][x], 0);
	}
}

//a and b should be the same type
void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

void draw_line(Frame grid, zbuffer buf, struct Pixel *p,
		int x1, int y1, float z1,
		int x2, int y2, float z2) {
	//make sure x1 y1 is the left point
	if (x1 > x2) {
		swap(&x1, &x2);
		swap(&y1, &y2);
		
		float temp = z1;
		z1 = z2;
		z2 = temp;
	}
	int a = y2-y1;
	int b = -(x2-x1);
	
	//d is for x or y, dz is for z only
	int d;
	float dz;
	
	float m = ((float)a)/(-b);
	//printf("%d, %d %f\n", a, -b, m);
	
	//octant 1
	if (m >= 0 && m <= 1) {
		a = y2-y1;
		b = -(x2-x1);
		
		d = 2*a + b;
		dz = (z2-z1)/(x2-x1);
		//printf("1\n");
		
		while (x1 <= x2 && x1 < IMG_WIDTH) {
			plot_point(grid, buf, x1, y1, z1, p);
			
			if (d > 0) {
				y1++;
				d += 2*b;
			}
			
			x1++;
			z1 += dz;
			d += 2*a;
		}
	}
	//octant 2
	else if (m > 1) {
		if (y1 > y2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
			
			float temp = z1;
			z1 = z2;
			z2 = temp;
		}
		a = y2-y1;
		b = -(x2-x1);
		
		d = a + 2*b;
		dz = (z2-z1)/(y2-y1);
		//printf("2\n");
		
		while (y1 <= y2 && y1 < IMG_HEIGHT) {
			plot_point(grid, buf, x1, y1, z1, p);
			
			if (d < 0) {
				x1++;
				d += 2*a;
			}
			
			y1++;
			z1 += dz;
			d += 2*b;
		}
	}
	//octant 8
	else if (m < 0 && m >= -1) {
		a = y2-y1;
		b = -(x2-x1);
		
		d = 2*a - b;
		dz = (z2-z1)/(x2-x1);
		//printf("8\n");
		
		while (x1 <= x2 && x1 < IMG_WIDTH) {
			plot_point(grid, buf, x1, y1, z1, p);
			
			if (d < 0) {
				y1--;
				d -= 2*b;
			}
			
			x1++;
			z1 += dz;
			d += 2*a;
		}
	}
	//octant 7
	else if (m < -1) {
		if (y1 < y2) {
			swap(&x1, &x2);
			swap(&y1, &y2);
			
			float temp = z1;
			z1 = z2;
			z2 = temp;
		}
		a = y2-y1;
		b = -(x2-x1);
		
		d = a - 2*b;
		dz = (z2-z1)/(y2-y1);
		//printf("7\n");
		
		while (y1 >= y2 && y1 < IMG_HEIGHT) {
			plot_point(grid, buf, x1, y1, z1, p);
			
			if (d > 0) {
				x1++;
				d += 2*a;
			}
			
			y1--;
			z1 -= dz;
			d -= 2*b;
		}
	}
}

void draw_polygons(Frame f, zbuffer buf, struct Matrix *m, struct Pixel *p,
		struct Light *l, float *view_vect) {
	int x;
	float norm[3];
	struct Pixel *fill_color;
	//int r = 10, g = 34, b = 123;
	for (x = 0; x < m->back; x+=3) {
		find_norm(m, x, x+1, x+2, norm);
		normalize(norm);
		//backface culling: don't draw if polygon
		//not in sight
		if (norm[2] > 0) {
			/*
			draw_line(f, p,
				m->m[0][x],
				m->m[1][x],
				m->m[0][(x+1)],
				m->m[1][(x+1)]
			);
			draw_line(f, p,
				m->m[0][(x+1)],
				m->m[1][(x+1)],
				m->m[0][(x+2)],
				m->m[1][(x+2)]
			);
			draw_line(f, p,
				m->m[0][(x+2)],
				m->m[1][(x+2)],
				m->m[0][x],
				m->m[1][x]
			);
			*/
			
			/*pixel_color(&fill_color, r, g, b);
			r += 50;
			g += 40;
			b += 25;
			r = r % 255;
			g = g % 255;
			b = b % 255;*/
			fill_color = get_lighting(l, norm, view_vect, .5, .5, .5);
			render_scanlines(f, buf, m, fill_color, x, x+1, x+2);
		}
	}
}

//matrix should be a pointer to a struct Matrix
int compare_matrix_indices(const void *p1, const void *p2, void *matrix) {
	struct Matrix *m = (struct Matrix *)matrix;
	float res = m->m[1][*((int *)p1)] - m->m[1][*((int *)p2)];
	if (res < 0) return -1;
	else if (res > 0) return 1;
	return 0;
}

void render_scanlines(Frame f, zbuffer b, struct Matrix *m, struct Pixel *p,
		int p1, int p2, int p3) {
	//determine top and bottom
	int hi, lo, mid;	//index of top, bottom, middle
	int sorted[] = {p1, p2, p3};
	qsort_r(sorted, 3, sizeof(int), *compare_matrix_indices, m);
	lo = sorted[0];
	mid = sorted[1];
	hi = sorted[2];
	
	float x0 = m->m[0][lo], x1 = m->m[0][lo];
	float z0 = m->m[2][lo], z1 = m->m[2][lo];

	//how much to increment x by, d1 will change
	float 	d0 = (m->m[0][hi] - m->m[0][lo])/(m->m[1][hi] - m->m[1][lo]),
		d1 = (m->m[0][mid] - m->m[0][lo])/(m->m[1][mid] - m->m[1][lo]);
	
	//likewise for z
	float 	dz0 = (m->m[2][hi] - m->m[2][lo])/(m->m[1][hi] - m->m[1][lo]),
		dz1 = (m->m[2][mid] - m->m[2][lo])/(m->m[1][mid] - m->m[1][lo]);
	
	if (m->m[1][mid] - m->m[1][lo] < 1) {
		x1 = m->m[0][mid];
		d1 = (m->m[0][hi] - m->m[0][mid])/(m->m[1][hi] - m->m[1][mid]);
		
		z1 = m->m[2][mid];
		dz1 = (m->m[2][hi] - m->m[2][mid])/(m->m[1][hi] - m->m[1][mid]);
		
		if (fabsf(m->m[1][hi] - m->m[1][mid]) < 1) return;
	}
	
	int y;
	//printf("ylo: %d, ymid: %d, yhi: %d\n",
	//		(int)m->m[1][lo], (int)m->m[1][mid], (int)m->m[1][hi]);
	for (y = (int)m->m[1][lo]; y <= (int)m->m[1][hi]; y++) {
		//always go from x0 to x1
		//x1 is always on the side dealing with the middle
		/*
		printf("x0: %f, x1: %f, y: %d, d0: %f, d1: %f\n",
				x0, x1, y, d0, d1);
		printf("hi: %f, mid: %f\n",
				roundf(m->m[1][hi]), roundf(m->m[1][mid]));
		*/
		
		//swap delta1 at midpoint
		if (fabsf(y - m->m[1][mid]) < 1) {
			x1 =  m->m[0][mid];
			d1 = (m->m[0][hi] - m->m[0][mid])/
				(m->m[1][hi] - m->m[1][mid]);
			
			z1 = m->m[2][mid];
			dz1 = (m->m[2][hi] - m->m[2][mid])/
				(m->m[1][hi] - m->m[1][mid]);
			
			if (fabsf(m->m[1][hi] - m->m[1][mid]) < 1) break;
			//printf("swapped d1: %f\n", d1);
		}
		draw_line(f, b, p,
				(int)roundf(x0), y, z0,
				(int)roundf(x1), y, z1);
		
		x0 += d0;
		x1 += d1;
		z0 += dz0;
		z1 += dz1;
	}
}

void clear(Frame f, zbuffer b) {
	memset(f, 0, sizeof(Frame));
	
	int x, y;
	for (y = 0; y < IMG_HEIGHT; y++)
	for (x = 0; x < IMG_WIDTH; x++)
		b[y][x] = -FLT_MAX;
}

