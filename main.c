#include<float.h>

#include"include/draw.h"
#include"include/output.h"
#include"include/parser.h"
#include"include/rcs.h"
#include"include/lighting.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Please state an input file\n");
		exit(1);
	}
	struct Rcs_stack *s = new_rcs_stack(4);
	Frame f;
	zbuffer buf;
	clear(f, buf);
	
	struct Pixel p;
	pixel_color(&p, 255, 235, 205);
	
	parse_instructions(argv[1], s, f, buf);

	free_stack(s);
	
	return 0;
}

