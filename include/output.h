#ifndef OUTPUT_H
#define OUTPUT_H

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

#include"draw.h"

#define FILE_NAME	"picture.ppm"

//p must be the same dimension as the desired image
void write_to_file(Frame p);

void display(Frame f);

void save_png(Frame f, char *name);

#endif
