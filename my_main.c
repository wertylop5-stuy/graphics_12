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

void pass_one(int *tot_frames, char *anim_name, int max_len) {
	char vary_found = 0;
	int x = 0;
	while ( op[x].opcode != 0 ) {
	switch(op[x].opcode) {
		case FRAMES:
			(*tot_frames) = op[x].op.frames.num_frames;
			printf("frames found: %d\n", *tot_frames);
		break;
		
		case VARY:
			vary_found = 1;
			printf("vary found\n");
		break;
		
		case BASENAME:
			strncpy(anim_name, op[x].op.basename.p->name,
					max_len);
			printf("basename found: %s\n", anim_name);
		break;
	}
	x++;
	}
	
	if (vary_found && *tot_frames == -1) {
		fprintf(stderr, "ERROR: vary command found, but number of frames not specified\n");
		exit(1);
	}
}

struct vary_node** pass_two(int tot_frames) {
	struct vary_node **res = (struct vary_node **)malloc(
		tot_frames * sizeof(struct vary_node *));
	
	//keep tracks of the tail of the linked lists
	struct vary_node* tails[tot_frames];
	memset(tails, 0, sizeof(tails));
	
	int x;
	for (x = 0; x < tot_frames; x++) {
		res[x] = 0;
	}
	
	x = 0;
	while ( op[x].opcode != 0 ) {
	switch(op[x].opcode) {
	case VARY:
	{
		char *knob = op[x].op.vary.p->name;
		//printf("Accessing knob: %s, with len %lu\n", knob, strlen(knob));
		
		int cur_frame;
		float cur;	//cuz frames are floats?
		float step = (op[x].op.vary.end_val - op[x].op.vary.start_val) /
			(op[x].op.vary.end_frame - op[x].op.vary.start_frame);
		for (cur = op[x].op.vary.start_frame;
				cur <= op[x].op.vary.end_frame;
				cur++) {
			cur_frame = (int)cur;
			/*
			 * If null, create a element and edit it directly.
			 * 
			 * If not null, create new element and 
			 * have prev->next point to new element
			 * */
			if (res[cur_frame] == 0) {
				res[cur_frame] = (struct vary_node *)malloc(sizeof(struct vary_node));
				strncpy(res[cur_frame]->name, knob, strlen(knob));
				res[cur_frame]->value = op[x].op.vary.start_val+
					(step*(cur-op[x].op.vary.start_frame));
				res[cur_frame]->next = 0;
				tails[cur_frame] = res[cur_frame];
			}
			else {
				struct vary_node *temp = (struct vary_node *)malloc(sizeof(struct vary_node));
				strncpy(temp->name, knob, strlen(knob));
				temp->value = op[x].op.vary.start_val+
					(step*(cur-op[x].op.vary.start_frame));
				temp->next = 0;
				
				tails[cur_frame]->next = temp;
				tails[cur_frame] = temp;
			}
		}
	}
	break;
	}
	x++;
	}
	
	x = 0;
	for (; x < tot_frames; x++) {
		//printf("Frame %d\n", x);
		if (res[x] != 0) {
			struct vary_node const * temp = res[x];
			while (temp != 0) {
				//printf("knob %s with value %f\n", temp->name, temp->value);
				temp = temp->next;
			}
		}
	}
	
	return res;
}

void process_knobs(struct vary_node **knobs, int cur_frame) {
	struct vary_node *temp = knobs[cur_frame];
	while (temp != 0) {
		printf("on knob: %s\n", temp->name);
		set_value(lookup_symbol(temp->name), temp->value);
		temp = temp->next;
	}
}

void my_main() {
	int tot_frames = -1;	//if this still = -1, then user doesn't want animation
	char anim_name[128];
	strncpy(anim_name, "default", sizeof(anim_name));
	
	//look for animation commands
	pass_one(&tot_frames, anim_name, 128);
	struct vary_node **knobs;
	if (tot_frames > 0) {
		knobs = pass_two(tot_frames);
	}
	
	//struct Matrix *m = new_matrix(4, 1000);
	struct Rcs_stack *s;
	struct Light *l;
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
		
	int cur_frame;
	char frame_name[256];
	if (tot_frames == -1) tot_frames = 1;
	for (cur_frame = 0; cur_frame < tot_frames; cur_frame++) {
	s = new_rcs_stack(3);
	l = new_light(67, 132, 75, 0, 255, 0, 1, 1, 1);
	clear(f, z);
	
	printf("frame %d\n", cur_frame);
	if (tot_frames > 1) {
		process_knobs(knobs, cur_frame);
	}
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
			struct Matrix *t = move(
				temp[0] * (op[x].op.move.p ?
					op[x].op.move.p->s.value :
					1),
				temp[1] * (op[x].op.move.p ?
					op[x].op.move.p->s.value :
					1),
				temp[2] * (op[x].op.move.p ?
					op[x].op.move.p->s.value :
					1));
			
			matrix_mult(peek(s), t);
			free_matrix(peek(s));
			s->stack[s->top] = copy_matrix(t);
			
			free_matrix(t);
		break;
		}
		
		case SCALE:
		{
			double *temp = op[x].op.scale.d;
			struct Matrix *t = scale(
				temp[0] * (op[x].op.scale.p ?
					op[x].op.scale.p->s.value :
					1),
				temp[1] * (op[x].op.scale.p ?
					op[x].op.scale.p->s.value :
					1),
				temp[2] * (op[x].op.scale.p ?
					op[x].op.scale.p->s.value :
					1));
			
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
				op[x].op.rotate.degrees *
			       		(op[x].op.rotate.p ?
					op[x].op.rotate.p->s.value :
					1) 
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
	if (tot_frames > 1) {
		//save the frame
		memset(frame_name, 0, sizeof(frame_name));
		snprintf(frame_name, sizeof(frame_name), "%s%04d%s.png", FRAME_DIR, cur_frame, anim_name);
		printf("%s\n", frame_name);
		save_png(f, frame_name);
	}
	
	free_light(l);
	free_stack(s);
	}
	
	if (tot_frames > 1) {
		save_anim(anim_name, FRAME_DIR);
		view_anim(anim_name, FRAME_DIR);
	}
	
	//free_light(l);
	//free_stack(s);
	//free_matrix(m);
}
