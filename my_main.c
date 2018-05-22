/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser,
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  push: push a new origin matrix onto the origin stack

  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix
                     based on the provided values, then
                     multiply the current top of the
                     origins stack by it.

  box/sphere/torus: create a solid object based on the
                    provided values. Store that in a
                    temporary matrix, multiply it by the
                    current top of the origins stack, then
                    call draw_polygons.

  line: create a line based on the provided values. Store
        that in a temporary matrix, multiply it by the
        current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the screen
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "compiler/parser.h"

#include "include/draw.h"
#include "include/rcs.h"
#include "include/shapes.h"
#include "include/output.h"

#include "compiler/symtab.h"

#include "y.tab.h"

void my_main() {
	struct Matrix *m = new_matrix(4, 1000);
	struct Rcs_stack *s = new_rcs_stack(3);
	struct Light *l = new_light(67, 132, 75, 0, 255, 0, 1, 1, 1);
	float view_vect[] = {0, 0, 1};
	
	Frame f;
	zbuffer z;
	struct Pixel pixel;
	float aReflect[3];
	float dReflect[3];
	float sReflect[3];
	//float step = 15;
	//float theta;
	
	aReflect[RED] = 0.1;
	aReflect[GREEN] = 0.1;
	aReflect[BLUE] = 0.1;

	dReflect[RED] = 0.5;
	dReflect[GREEN] = 0.5;
	dReflect[BLUE] = 0.5;

	sReflect[RED] = 0.5;
	sReflect[GREEN] = 0.5;
	sReflect[BLUE] = 0.5;
	
	clear(f, z);
	pixel_color(&pixel, 0, 0, 0);
		
	int x = 0;
	while ( op[x].opcode != 0 ) {
	switch(op[x].opcode) {
		case PUSH:
			push_rcs(s);
		break;
			
		case POP:
			pop_rcs(s);
		break;
		
		case MOVE:
		{
			double *temp = op[x].op.move.d;
			struct Matrix *t = move(temp[0], temp[1], temp[2]);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		break;
		}
		
		case SCALE:
		{
			double *temp = op[x].op.scale.d;
			struct Matrix *t = scale(temp[0], temp[1], temp[2]);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		break;
		}
		
		case ROTATE:
		{
			//why is axis defined as a double lmao
			struct Matrix *t = rotate(
					op[x].op.rotate.axis,
					op[x].op.rotate.degrees
					);
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		break;
		}
			
		case BOX:
		{
			double *temp = op[x].op.box.d0;
			double *temp2 = op[x].op.box.d1;
			struct Matrix *p = new_matrix(4, 1);
			
			add_cube(p, temp[0], temp[1], temp[2],
				temp2[0], temp2[1], temp2[2]);
			matrix_mult(peek(s), p);
			draw_polygons(f, z, p, &pixel, l, view_vect);
			free_matrix(p);
		break;
		}
		
		case SPHERE:
		{
			double *temp = op[x].op.sphere.d;
			struct Matrix *p = new_matrix(4, 1);
			
			add_sphere(p, temp[0], temp[1], temp[2],
				op[x].op.sphere.r, 12);
			matrix_mult(peek(s), p);
			draw_polygons(f, z, p, &pixel, l, view_vect);
			free_matrix(p);
		break;
		}
		
		case TORUS:
		{
			double *temp = op[x].op.torus.d;
			struct Matrix *p = new_matrix(4, 1);
					
			add_torus(p, temp[0], temp[1], temp[2],
				op[x].op.torus.r0,
				op[x].op.torus.r1,
				15);
			matrix_mult(peek(s), p);
			draw_polygons(f, z, p, &pixel, l, view_vect);
			free_matrix(p);
		break;
		}
		
		case LINE:
		{
			double *temp  = op[x].op.line.p0;
			double *temp2 = op[x].op.line.p1;
			struct Matrix *e = new_matrix(4, 1);
			
			push_edge(e, temp[0], temp[1], temp[2],
				temp2[0], temp2[1], temp2[2]);
			matrix_mult(peek(s), e);
			draw_lines(f, z, e, &pixel);
			free_matrix(e);
		break;
		}
			
		case SAVE:
			write_to_file(f);
			save_png(f, op[x].op.save.p->name);
		break;
		
		case DISPLAY:
			display(f);
		break;
	};
	x++;
	}
	
	free_light(l);
	free_stack(s);
	free_matrix(m);
}
