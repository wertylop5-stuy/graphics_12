#include"include/lighting.h"

struct Light* new_light(float aR, float aG, float aB,
		float pR, float pG, float pB,
		float x, float y, float z) {
	struct Light *res = (struct Light *)malloc(sizeof(struct Light)); 
	res->ambient_color =
		(struct Pixel *)malloc(sizeof(struct Pixel)); 
	res->point_color =
		(struct Pixel *)malloc(sizeof(struct Pixel)); 

	res->ambient_color->r = aR;
	res->ambient_color->g = aG;
	res->ambient_color->b = aB;
	
	res->point_color->r = pR;
	res->point_color->g = pG;
	res->point_color->b = pB;

	res->light_vector[0] = x;
	res->light_vector[1] = y;
	res->light_vector[2] = z;
	
	normalize(res->light_vector);
	
	return res;
}

void free_light(struct Light *l) {
	free(l->ambient_color);
	free(l->point_color);
	free(l);
}

//normal and view are 3 element arrays for the corresponding vector
struct Pixel* get_lighting(struct Light *l, float *normal, float *view,
		float aReflect, float dReflect, float sReflect) {
	struct Pixel *a = calc_ambient(l, aReflect);
	struct Pixel *d = calc_diffuse(l, normal, dReflect);
	struct Pixel *s = calc_specular(l, normal, view, sReflect);
	
	//don't go above 255
	a->r = fminf(a->r + d->r + s->r, 255);
	a->g = fminf(a->g + d->g + s->g, 255);
	a->b = fminf(a->b + d->b + s->b, 255);
	
	free(d);
	free(s);
	
	return a;
}

struct Pixel* calc_ambient(struct Light *l, float aReflect) {
	struct Pixel *res = (struct Pixel *)malloc(sizeof(struct Pixel));

	res->r = l->ambient_color->r * aReflect;
	res->g = l->ambient_color->g * aReflect;
	res->b = l->ambient_color->b * aReflect;
	
	return res;
}

struct Pixel* calc_diffuse(struct Light *l, float *normal, float dReflect) {
	struct Pixel *res = (struct Pixel *)malloc(sizeof(struct Pixel));
	float dot = dot_product(normal, l->light_vector);
	
	//negative means its facing away from the light
	if (dot < 0) {
		res->r = res->g = res->b = 0;
		return res;
	}
	
	res->r = l->point_color->r * dReflect * dot;
	res->g = l->point_color->g * dReflect * dot;
	res->b = l->point_color->b * dReflect * dot;
	
	return res;
}

struct Pixel* calc_specular(struct Light *l, float *normal, float *view,
		float sReflect) {
	struct Pixel *res = (struct Pixel *)malloc(sizeof(struct Pixel));
	float dot = dot_product(normal, l->light_vector);
	
	if (dot < 0) {
		res->r = res->g = res->b = 0;
		return res;
	}
	
	float temp[] = {
		normal[0]*2*dot - l->light_vector[0],
		normal[1]*2*dot - l->light_vector[1],
		normal[2]*2*dot - l->light_vector[2]
	};
	
	dot = dot_product(temp, view);
	if (dot < 0) {
		res->r = res->g = res->b = 0;
		return res;
	}
	
	dot = powf(dot, 4);
	
	res->r = l->point_color->r * sReflect * dot;
	res->g = l->point_color->g * sReflect * dot;
	res->b = l->point_color->b * sReflect * dot;
	
	return res;
}

