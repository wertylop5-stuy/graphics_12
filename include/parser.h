#ifndef PARSER_H
#define PARSER_H

#include<stdio.h>
#include<string.h>

#include"draw.h"
#include"output.h"
#include"shapes.h"
#include"rcs.h"

//t is transform, p is polygon, e is edge
void parse_instructions(char *filename, struct Rcs_stack *s, Frame f, zbuffer b);

#endif
