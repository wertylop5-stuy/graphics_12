#include"include/parser.h"

void parse_instructions(char *filename, struct Rcs_stack *s, Frame f, zbuffer b) {
	FILE *file = fopen(filename, "r");
	if (!file) return;
	
	char line[256];
	char name[128];
	float x1, y1, z1, x2, y2, z2;
	char axis;
	
	memset(f, 0, sizeof(Frame));
	struct Pixel pixel;
	pixel_color(&pixel, 255, 105, 180);
	
	struct Light *l = new_light(67, 132, 75, 0, 255, 0, 1, 1, 1);
	float view_vect[] = {0, 0, 1};
	
	while (fgets(line, sizeof(line), file) != 0 && line[0] != '\n') {
		if (!strncmp(line, "line", strlen(line)-1)) {
			struct Matrix *e = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f %f %f",
				&x1, &y1, &z1, &x2, &y2, &z2);
			
			push_edge(e, x1, y1, z1, x2, y2, z2);
			matrix_mult(peek(s), e);
			draw_lines(f, b, e, &pixel);
			free_matrix(e);
		}
		else if (!strncmp(line, "scale", strlen(line)-1)) {
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f",
				&x1, &y1, &z1);
			
			struct Matrix *t = scale(x1, y1, z1);
			//matrix_mult(t, s->stack[s->top]);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		}
		else if (!strncmp(line, "move", strlen(line)-1)) {
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f",
				&x1, &y1, &z1);
			
			struct Matrix *t = move(x1, y1, z1);
			//matrix_mult(t, s->stack[s->top]);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		}
		else if (!strncmp(line, "rotate", strlen(line)-1)) {
			fgets(line, sizeof(line), file);
			sscanf(line, "%c %f",
				&axis, &z1);
			
			struct Matrix *t = rotate(axis, z1);
			//matrix_mult(t, s->stack[s->top]);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		}
		else if (!strncmp(line, "display", strlen(line)-1)) {
			display(f);
		}
		else if (!strncmp(line, "save", strlen(line)-1)) {
			fgets(line, sizeof(line), file);
			sscanf(line, "%s", name);
			write_to_file(f);
			save_png(f, name);
		}
		else if (!strncmp(line, "quit", strlen(line)-1)) {
			break;
			fgets(line, sizeof(line), file);
			sscanf(line, "%s", name);
		}
		else if (!strncmp(line, "circle", strlen(line)-1)) {
			float cx, cy, cz, r;
			struct Matrix *e = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f", &cx, &cy, &cz, &r);
			
			make_circle(e, cx, cy, cz, r, 2*M_PI);
			matrix_mult(peek(s), e);
			draw_lines(f, b, e, &pixel);
			free_matrix(e);
		}
		else if (!strncmp(line, "hermite", strlen(line)-1)) {
			float x0, y0, x1, y1, rx0, ry0, rx1, ry1;
			struct Matrix *e = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f %f %f %f %f",
				&x0, &y0, &x1, &y1, &rx0, &ry0, &rx1, &ry1);
			
			make_hermite(e, x0, y0, x1, y1, rx0, ry0, rx1, ry1);
			matrix_mult(peek(s), e);
			draw_lines(f, b, e, &pixel);
			free_matrix(e);
		}
		else if (!strncmp(line, "bezier", strlen(line)-1)) {
			float x0, y0, x1, y1, x2, y2, x3, y3;
			struct Matrix *e = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f %f %f %f %f",
				&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3);
			
			make_bezier(e, x0, y0, x1, y1, x2, y2, x3, y3);
			matrix_mult(peek(s), e);
			draw_lines(f, b, e, &pixel);
			free_matrix(e);
		}
		else if (!strncmp(line, "sphere", strlen(line)-1)) {
			float x, y, z, r;
			struct Matrix *p = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f",
				      &x, &y, &z, &r);
			
			//draw sphere then remove the matrix
			add_sphere(p, x, y, z, r, 12);
			matrix_mult(peek(s), p);
			draw_polygons(f, b, p, &pixel, l, view_vect);
			free_matrix(p);
		}
		else if (!strncmp(line, "box", strlen(line)-1)) {
			float x, y, z, w, h, d;
			struct Matrix *p = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f %f %f",
				      &x, &y, &z, &w, &h, &d);
			
			add_cube(p, x, y, z, w, h, d);
			matrix_mult(peek(s), p);
			draw_polygons(f, b, p, &pixel, l, view_vect);
			free_matrix(p);
		}
		else if (!strncmp(line, "torus", strlen(line)-1)) {
			float x, y, z, r1, r2;
			struct Matrix *p = new_matrix(4, 1);
			
			fgets(line, sizeof(line), file);
			sscanf(line, "%f %f %f %f %f",
				      &x, &y, &z, &r1, &r2);
			
			add_torus(p, x, y, z, r1, r2, 15);
			matrix_mult(peek(s), p);
			draw_polygons(f, b, p, &pixel, l, view_vect);
			free_matrix(p);
		}
		else if (!strncmp(line, "push", strlen(line)-1)) {
			push_rcs(s);
		}
		else if (!strncmp(line, "pop", strlen(line)-1)) {
			pop_rcs(s);
		}
		else if (!strncmp(line, "clear", strlen(line)-1)) {
			clear(f, b);
		}
	}
	
	free_light(l);
	fclose(file);
	file = 0;
}

