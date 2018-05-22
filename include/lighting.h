/*
 * lighting.h
 *
 * Contains functionality for calculating surface colors
 * on polygons.
 *
 * Uses the Phong reflection model.
 * */

#ifndef LIGHTING_H
#define LIGHTING_H

#define RED	0
#define GREEN	1
#define BLUE	2

#include"draw.h"

//ambient, diffuse, specular
struct Light {
	struct Pixel *ambient_color;
	struct Pixel *point_color;
	
	//location of the point light source
	float light_vector[3];
};

/*
 * First three args are RGB values for the ambient light.
 * Second three args are RGB values for the point light.
 * Last three args are coordinates for the light vector.
 * */
struct Light* new_light(float aR, float aG, float aB,
		float pR, float pG, float pB,
		float x, float y, float z);

void free_light(struct Light *l);

//normal and view are 3 element arrays for the corresponding vector
struct Pixel* get_lighting(struct Light *l, float *normal, float *view,
		float aReflect, float dReflect, float sReflect);

struct Pixel* calc_ambient(struct Light *l, float aReflect);
struct Pixel* calc_diffuse(struct Light *l, float *normal, float dReflect);
struct Pixel* calc_specular(struct Light *l, float *normal, float *view,
		float sReflect);

#endif

