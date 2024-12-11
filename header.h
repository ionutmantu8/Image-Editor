#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
//Copyright Ionut Mantu 313 CAb 2023-2024
typedef struct
{
	int red;
	int green;
	int blue;
} rgb;

typedef struct
{
	char type[3];
	int cols;
	int rows;
	int n;
	rgb **matrix;
	int select_x1, select_x2, select_y1, select_y2;
} image;
int clamp(int num);

int command(char *com, char *com_extra);


image *load(const char *filename);

void save(const char *filename, image *img, const char *ascii);

void freeimage(image *img);

void select_area(image *img, char **words);

void free_matrix_rgb(rgb **matrix, int rows);

void crop(image *img);

void select_all(image *img);

void equalize(image *img);

void histogram(image *img, int stars, int bins);

void exit_program(image *img);

void verify_histo(image *img, char **words);

int verify_apply(char *filter);

void apply(image *img, char *filter);

void rotate90_gray_right(image *image);

void rotate_90_gray_left(image *img);

void rotate90_rgb_right(image *image);

void rotate_rgb_90_left(image *img);

void rotate90_gray_right_select(image *image);

void rotate_90_gray_left_select(image *img);

void rotate90_rgb_right_select(image *image);

void rotate_90_rgb_left_select(image *img);
	
void rotate(image *img,char **words);
