#include"include/output.h"

void write_to_file(Frame p) {
	int fd = open(FILE_NAME, O_CREAT | O_WRONLY, 0644);
	char buf[1024];
	
	sprintf(buf, "P3\n%d %d\n%d\n", IMG_WIDTH, IMG_HEIGHT, MAX_COLOR);
	write(fd, buf, strlen(buf));
	
	int x, y;
	for (y = 0; y < IMG_HEIGHT; y++) {
	for (x = 0; x < IMG_WIDTH; x++) {
		sprintf(buf, "%d %d %d ", p[y][x].r, p[y][x].g, p[y][x].b);
		write(fd, buf, strlen(buf));
	}
		sprintf(buf, "\n");
		write(fd, buf, strlen(buf));
	}
}

//from Mr. DW's code
void display(Frame f) {
	FILE *file;
	file = popen("display", "w");

	fprintf(file, "P3\n%d %d\n%d\n", IMG_WIDTH, IMG_HEIGHT, MAX_COLOR);

	int x,y;
	for (y = 0; y < IMG_HEIGHT; y++) {
	for (x = 0; x < IMG_WIDTH; x++) {
		fprintf(file, "%d %d %d ", f[y][x].r, f[y][x].g, f[y][x].b);
	}
		fprintf(file, "\n");
	}
	pclose(file);
}

void save_png(Frame f, char *name) {
	char temp[128];
	sprintf(temp, "convert - %s", name);

	FILE *file;
	file = popen(temp, "w");
	
	fprintf(file, "P3\n%d %d\n%d\n", IMG_WIDTH, IMG_HEIGHT, MAX_COLOR);
	
	int x,y;
	for (y = 0; y < IMG_HEIGHT; y++) {
	for (x = 0; x < IMG_WIDTH; x++) {
		fprintf(file, "%d %d %d ", f[y][x].r, f[y][x].g, f[y][x].b);
	}
		fprintf(file, "\n");
	}
	pclose(file);
}

