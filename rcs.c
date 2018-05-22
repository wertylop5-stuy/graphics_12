#include"include/rcs.h"

struct Rcs_stack* new_rcs_stack(int size) {
	struct Rcs_stack *res = (struct Rcs_stack *)malloc(sizeof(struct Rcs_stack));
	res->stack = (struct Matrix **)malloc( (size+1)*sizeof(struct Matrix *));
	res->capacity = size+1;
	res->top = 1;
	
	//add base coordinate system
	res->stack[0] = new_matrix(4, 4);
	ident(res->stack[0]);
	res->stack[0]->back = 4;

	//add the working version
	res->stack[1] = copy_matrix(res->stack[0]);
	
	return res;
}

void print_stack(struct Rcs_stack *s) {
	int x;
	for (x = 0; x <= s->top; x++) {
		printf("coordinate system at %d\n", x);
		print_matrix(s->stack[x]);
	}
}

void free_stack(struct Rcs_stack *s) {
	int x;
	for (x = 0; x <= s->top; x++) {
		free_matrix(s->stack[x]);
	}
	free(s->stack);
	free(s);
}

void resize_stack(struct Rcs_stack *s) {
	s->stack = realloc(s->stack, ((s->capacity)*=2)*sizeof(struct Matrix *) );
}

struct Matrix *peek(struct Rcs_stack *s) {
	return s->stack[s->top];
}

void push_rcs(struct Rcs_stack *s) {
	if (s->top+1 >= s->capacity) resize_stack(s);
	s->stack[(s->top)+1] = copy_matrix(s->stack[s->top]);
	(s->top)+=1;
}

void pop_rcs(struct Rcs_stack *s) {
	//can't go past the base coordinate system
	if (s->top == 1) {
		free_matrix(s->stack[1]);
		s->stack[1] = copy_matrix(s->stack[0]);
		return;
	}
	free_matrix(s->stack[s->top]);
	(s->top)--;
}

