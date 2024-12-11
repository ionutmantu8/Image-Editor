#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "header.h"
//Copyright Ionut Mantu 313 CAb 2023-2024

int clamp(int num)
{
	if (num < 0)
		return 0;
	if (num > 255)
		return 255;
	return num;
}
int command(char *com, char *com_extra)
{
	if (strcmp("LOAD", com) == 0)
		return 0;
	if (strcmp("SAVE", com) == 0)
		return 1;
	if (strcmp("CROP", com) == 0)
		return 2;
	if (strcmp("EXIT", com) == 0)
		return 3;
	if (strcmp("HISTOGRAM", com) == 0)
		return 5;
	if (strcmp("SELECT", com) == 0)
	{
		if (strcmp("ALL", com_extra) == 0)
		{
			return 4;
		}
		return 6;
	}
	if (strcmp("ROTATE", com) == 0)
		return 7;
	if (strcmp("APPLY", com) == 0)
		return 8;
	if (strcmp("EQUALIZE", com) == 0)
		return 9;
	return -1;
}

image *load(const char *filename)
{
	image *img = malloc(sizeof(image));

	FILE *file = fopen(filename, "r");
	if (file == NULL)
		return NULL;

	fscanf(file, "%s", img->type);
	fscanf(file, "%d %d", &img->cols, &img->rows);
	fscanf(file, "%d", &img->n);
	fclose(file);
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P3") == 0) {
		file = fopen(filename, "r");
	}
	else if (strcmp(img->type, "P5") == 0 || strcmp(img->type, "P6") == 0) {
		file = fopen(filename, "rb");
	}
	fscanf(file, "%s", img->type);
	fscanf(file, "%d %d", &img->cols, &img->rows);
	fscanf(file, "%d", &img->n);
	fgetc(file);
	img->matrix = malloc(img->rows * sizeof(rgb *));
	for (int i = 0; i < img->rows; i++) {
		img->matrix[i] = malloc(img->cols * sizeof(rgb));
	}

	// Read pixel data based on image type
	if (strcmp(img->type, "P2") == 0) {
		int aux;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				fscanf(file, "%d", &aux);
				img->matrix[i][j].red = aux;
			}
		}
	}
	else if (strcmp(img->type, "P3") == 0) {
		int aux1;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				fscanf(file, "%d", &aux1);
				img->matrix[i][j].red = aux1;
				fscanf(file, "%d", &aux1);
				img->matrix[i][j].green = aux1;
				fscanf(file, "%d", &aux1);
				img->matrix[i][j].blue = aux1;
			}
		}
	}
	else if (strcmp(img->type, "P5") == 0) {
		unsigned char aux2;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				fread(&aux2, sizeof(unsigned char), 1, file);
				img->matrix[i][j].red = (int)aux2;
			}
		}
	}
	else if (strcmp(img->type, "P6") == 0) {
		unsigned char aux3;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				fread(&aux3, sizeof(unsigned char), 1, file);
				img->matrix[i][j].red = (int)aux3;
				fread(&aux3, sizeof(unsigned char), 1, file);
				img->matrix[i][j].green = (int)aux3;
				fread(&aux3, sizeof(unsigned char), 1, file);
				img->matrix[i][j].blue = (int)aux3;
			}
		}
	}
	img->select_x1 = 0;
	img->select_x2 = 0;
	img->select_y1 = 0;
	img->select_y2 = 0;
	fclose(file);	

	printf("Loaded %s\n", filename);
	return img;
}

void save(const char *filename, image *img, const char *ascii)
{
	FILE *file;

	file = fopen(filename, "wb");
	if (file == NULL) {
		printf("Failed to open file for writing.\n");
		return;
	}

	if (img->type[1] == '2' || img->type[1] == '3') {
		img->type[1] += 3;
	}

	if (ascii != NULL) {
		if (strcmp(ascii, "ascii") == 0) {
			img->type[1] -= 3;
		}
		else {
			printf("Invalid command\n");
		}
	}

	fprintf(file, "%s\n", img->type);
	fprintf(file, "%d %d\n", img->cols, img->rows);
	fprintf(file, "%d\n", img->n);

	if (strcmp(img->type, "P2") == 0) {

		int aux;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				aux = (int)img->matrix[i][j].red;
				fprintf(file, "%d ", aux);
			}
		}
	}
	else if (strcmp(img->type, "P3") == 0) {

		int aux1;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				aux1 = (int)img->matrix[i][j].red;
				fprintf(file, "%d ", aux1);
				aux1 = (int)img->matrix[i][j].green;
				fprintf(file, "%d ", aux1);
				aux1 = (int)img->matrix[i][j].blue;
				fprintf(file, "%d ", aux1);
			}
		}
	}
	else if (strcmp(img->type, "P5") == 0) {

		for (int i = 0; i < img->rows; i++){
			for (int j = 0; j < img->cols; j++)
			{
				fwrite(&img->matrix[i][j].red, sizeof(unsigned char), 1, file);
			}
		}
	}
	else if (strcmp(img->type, "P6") == 0) {

		unsigned char aux3;
		for (int i = 0; i < img->rows; i++) {
			for (int j = 0; j < img->cols; j++) {
				aux3 = img->matrix[i][j].red;
				fwrite(&aux3, sizeof(unsigned char), 1, file);
				aux3 = img->matrix[i][j].green;
				fwrite(&aux3, sizeof(unsigned char), 1, file);
				aux3 = img->matrix[i][j].blue;
				fwrite(&aux3, sizeof(unsigned char), 1, file);
			}
		}
	}
	printf("Saved %s\n", filename);
	fclose(file);
}

// Funcție pentru eliberarea completă a memoriei la ieșire
void freeimage(image *img)
{
	if (img == NULL)
		return;

	for (int i = 0; i < img->rows; i++) {
		free(img->matrix[i]);
	}
	free(img->matrix);
	free(img);
}

int isnumeric(const char *str)
{
    if (*str == '-') {
        str++; // increment pointer to check the rest of the string
    }

    while(*str != '\0') {
        if(!isdigit(*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

void select_area(image *img, char **words)
{
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}

	if (words[1] == NULL || words[2] == NULL || words[3] == NULL || words[4] == NULL) {
		printf("Invalid command\n");
		return;
	}

	if (!isnumeric(words[1]) || !isnumeric(words[2]) || !isnumeric(words[3]) || !isnumeric(words[4])) {
		printf("Invalid command\n");
		return;
	}

	int x1 = atoi(words[1]);
	int y1 = atoi(words[2]);
	int x2 = atoi(words[3]);
	int y2 = atoi(words[4]);
	
	if (x1 > x2) {
		int temp = x1;
		x1 = x2;
		x2 = temp;
	}
	if (y1 > y2) {
		int temp = y1;
		y1 = y2;
		y2 = temp;
	} 
	if (x1 < 0 || x2 > img->cols || y1 < 0 || y2 > img->rows) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if(x1==x2 || y1==y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	if (x1 == x2 && y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}
	
	img->select_x1 = x1;
	img->select_x2 = x2;
	img->select_y1 = y1;
	img->select_y2 = y2;

	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

void free_matrix_rgb(rgb **matrix, int rows) 
{
	int i;
	for (i = 0; i < rows; i++)
		free(matrix[i]);
	free(matrix);
}

void crop(image *img)
{
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}

	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		rgb **aux = malloc((img->select_y2 - img->select_y1) * sizeof(rgb *));
		int i, j;
		for (i = 0; i < (img->select_y2 - img->select_y1); i++)
			aux[i] = malloc((img->select_x2 - img->select_x1) * sizeof(rgb));
		for (i = 0; i < (img->select_y2 - img->select_y1); i++)
			for (j = 0; j < (img->select_x2 - img->select_x1); j++)
				aux[i][j] = img->matrix[i + img->select_y1][j + img->select_x1];
		free_matrix_rgb(img->matrix, img->rows);
		img->rows = img->select_y2 - img->select_y1;
		img->cols = img->select_x2 - img->select_x1;
		img->matrix = malloc(img->rows * sizeof(rgb *));
		for (i = 0; i < img->rows; i++)
			img->matrix[i] = malloc(img->cols * sizeof(rgb));
		for (i = 0; i < img->rows; i++)
			for (j = 0; j < img->cols; j++)
				img->matrix[i][j] = aux[i][j];
		free_matrix_rgb(aux, img->rows);
		printf("Image cropped\n");
		img->select_x1 = 0;
		img->select_x2 = img->cols;
		img->select_y1 = 0;
		img->select_y2 = img->rows;
	}
	else if (strcmp(img->type, "P3") == 0 || strcmp(img->type, "P6") == 0)
	{
		rgb **aux1 = malloc((img->select_y2 - img->select_y1) * sizeof(rgb *));
		int i, j;
		for (i = 0; i < (img->select_y2 - img->select_y1); i++)
			aux1[i] = malloc((img->select_x2 - img->select_x1) * sizeof(rgb));
		for (i = 0; i < (img->select_y2 - img->select_y1); i++)
			for (j = 0; j < (img->select_x2 - img->select_x1); j++)
				aux1[i][j] = img->matrix[i + img->select_y1][j + img->select_x1];
		free_matrix_rgb(img->matrix, img->rows);
		img->rows = img->select_y2 - img->select_y1;
		img->cols = img->select_x2 - img->select_x1;
		img->matrix = malloc(img->rows * sizeof(rgb *));
		for (i = 0; i < img->rows; i++)
			img->matrix[i] = malloc(img->cols * sizeof(rgb));
		for (i = 0; i < img->rows; i++)
			for (j = 0; j < img->cols; j++)
				img->matrix[i][j] = aux1[i][j];
		free_matrix_rgb(aux1, img->rows);
		img->select_x1 = 0;
		img->select_x2 = img->cols;
		img->select_y1 = 0;
		img->select_y2 = img->rows;
		printf("Image cropped\n");
	}
}

void select_all(image *img)
{

	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}
	img->select_x1 = 0;
	img->select_x2 = img->cols;
	img->select_y1 = 0;
	img->select_y2 = img->rows;
	printf("Selected ALL\n");
}

void equalize(image *img)
{
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}
	int sum;
	int area = img->rows * img->cols;
	if (img->type[1] == '3' || img->type[1] == '6') {
		printf("Black and white image needed\n");
		return;
	}
	int *frecv = calloc(256, sizeof(int));
	for (int i = 0; i < img->rows; i++)
		for (int j = 0; j < img->cols; j++) {
			int index = img->matrix[i][j].red;
			frecv[index]++;
		}
	for (int i = 0; i < img->rows; i++) {
		for (int j = 0; j < img->cols; j++) {
			sum = 0;
			for (int a = 0; a <= img->matrix[i][j].red; a++) {
				sum += frecv[a];
			}
			img->matrix[i][j].red = clamp(round((255.0 * sum) / area));
		}
	}
	printf("Equalize done\n");
	free(frecv);
}

void histogram(image *img, int stars, int bins) {
	if (img->type[1] == '3' || img->type[1] == '6') {
		printf("Black and white image needed\n");
		return;
	}
	if (bins < 2 && 256 % bins != 0) {
		printf("Invalid set of parameters\n");
		return;
	}
	int *frecv = calloc(256, sizeof(int));
	for (int i = 0; i < img->rows; i++)
		for (int j = 0; j < img->cols; j++) {
			frecv[img->matrix[i][j].red]++;
		}
	int groups = 256 / bins;
	int *aux = calloc(bins, sizeof(int));
	int max_value = 0;
	int x = 0;
	for (int i = 0; i < bins; i++) {
		for (int j = 0; j < groups; j++) {
			aux[i] += frecv[x];
			x++;
		}
		if (aux[i] > max_value)
			max_value = aux[i];
	}
	for (int i = 0; i < bins; i++) {
		aux[i] = (int)((stars * aux[i]) / max_value);
		printf("%d\t|\t", aux[i]);
		for (int j = 0; j < aux[i]; j++) {
			printf("*");
		}
		printf("\n");
	}
	free(aux);
	free(frecv);
}

void exit_program(image *img)
{
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}

	if (img->matrix) {
		for (int i = 0; i < img->rows; i++)
			free(img->matrix[i]);
		free(img->matrix);
	}
	free(img);
}

void verify_histo(image *img, char **words)
{
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}
	else {
		if ((words[1] != NULL && words[2] != NULL && words[3] != NULL) || (words[1] != NULL && words[2] == NULL)) {
			printf("Invalid command\n");
			return;
		}
		else {
			if (words[1] == NULL || words[2] == NULL) {
				printf("Invalid command\n");
				return;
			}
			else
				histogram(img, atoi(words[1]), atoi(words[2]));
		}
	}
}

int verify_apply(char *filter)
{
	if (filter == NULL)
		return 2;
	if(strcmp(filter,"EDGE")!=0&&strcmp(filter,"SHARPEN")!=0&&
	strcmp(filter,"BLUR")!=0&&strcmp(filter,"GAUSSIAN_BLUR")!=0)
		return 0;
	return 1;
	}

void apply(image *img, char *filter) {
	if (img == NULL) {
		printf("No image loaded\n");
		return;
	}
	int x=verify_apply(filter);
	if(x==0) {
		printf("APPLY parameter invalid\n");
		return;
	} else if(x==2) {
		printf("Invalid command\n");
		return;
	}
	
	if (strcmp(img->type, "P2") == 0 || strcmp(img->type, "P5") == 0) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}
	int ker[3][3];
	if (!strcmp(filter, "EDGE")) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                ker[i][j] = -1;
        ker[1][1] = 8;
    } else if (!strcmp(filter, "SHARPEN")) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                if ((i + j) % 2 == 0)
                    ker[i][j] = 0;
                else
                    ker[i][j] = -1;
        ker[1][1] = 5;
    } else if (!strcmp(filter, "BLUR")) {
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                ker[i][j] = 1;
    } else if (!strcmp(filter, "GAUSSIAN_BLUR")) {
        ker[0][0] = 1; 
		ker[0][1] = 2; 
		ker[0][2] = 1;
        ker[1][0] = 2; 
		ker[1][1] = 4; 
		ker[1][2] = 2;
        ker[2][0] = 1; 
		ker[2][1] = 2; 
		ker[2][2] = 1;
    }
	rgb **aux = calloc(img->rows,sizeof(rgb *));
    if (img->select_x1 == 0 && img->select_x2 == 0 && img->select_y1 == 0 && img->select_y2 == 0) {
        img->select_x1 = 0;
        img->select_x2 = img->cols;
        img->select_y1 = 0;
        img->select_y2 = img->rows;
    }
	int x1,y1,x2,y2;
	x1=img->select_x1;
	x2=img->select_x2;
	y1=img->select_y1;
	y2=img->select_y2;
	for (int i = 0; i < img->rows; i++)
		aux[i] = calloc(img->cols,sizeof(rgb));

	if (y2 == img->rows)
        y2--;
    if (x1 < 1)
        x1++;
    if (y1 < 1)
        y1++;
    if (x2 == img->cols)
    x2--;

	for (int i = y1; i < y2; i++)
		for (int j = x1; j < x2; j++)
			for (int m = 0,a=i-1; m < 3 && a<i+2; m++,a++)
				for (int n = 0,b=j-1;n < 3&& b<j+2; n++,b++) {

				aux[i][j].red += img->matrix[a][b].red*ker[m][n];

				aux[i][j].green += img->matrix[a][b].green*ker[m][n];

				aux[i][j].blue += img->matrix[a][b].blue*ker[m][n];

				
			
		}
if(strcmp(filter,"EDGE")==0){
	for (int i = y1; i < y2; i++)
		for (int j = x1; j <  x2; j++) {
			img->matrix[i][j].red = clamp(aux[i][j].red);
			img->matrix[i][j].green = clamp(aux[i][j].green);
			img->matrix[i][j].blue = clamp(aux[i][j].blue);
		}
printf("APPLY EDGE done\n");
}else if(strcmp(filter,"SHARPEN")==0){
	for (int i = y1; i < y2; i++)
		for (int j = x1; j <  x2; j++) {
			img->matrix[i][j].red = clamp(aux[i][j].red);
			img->matrix[i][j].green = clamp(aux[i][j].green);
			img->matrix[i][j].blue = clamp(aux[i][j].blue);
		}
printf("APPLY SHARPEN done\n");
}else if(strcmp(filter,"BLUR")==0){
	for (int i = y1; i < y2; i++)
		for (int j = x1; j <  x2; j++) {
			img->matrix[i][j].red = clamp(round((double)aux[i][j].red/9));
			img->matrix[i][j].green = clamp(round((double)aux[i][j].green/9));
			img->matrix[i][j].blue = clamp(round((double)aux[i][j].blue/9));
	
}
printf("APPLY BLUR done\n");
}else if(strcmp(filter,"GAUSSIAN_BLUR")==0){
	for (int i = y1; i < y2; i++)
		for (int j = x1; j <  x2; j++) {
			img->matrix[i][j].red = clamp(round((double)aux[i][j].red/16));
			img->matrix[i][j].green = clamp(round((double)aux[i][j].green/16));
			img->matrix[i][j].blue = clamp(round((double)aux[i][j].blue/16));
	
}
printf("APPLY GAUSSIAN_BLUR done\n");
}
free_matrix_rgb(aux, img->rows);
}
void rotate90_gray_right(image *image)
{
	rgb **aux_matrix = malloc(image->rows * sizeof(rgb *));
	for(int i = 0; i < image->rows; i++)
		aux_matrix[i] = malloc(image->cols * sizeof(rgb));
	for(int i = 0; i < image->rows; i++)
		for(int j = 0; j < image->cols; j++)
			aux_matrix[i][j].red = image->matrix[i][j].red;
	for (int i = 0; i < image->rows / 2; i++)
	{
		for (int j = i; j < image->rows - i - 1; j++)
		{
			int aux;
			aux = image->matrix[i][j].red;
			aux_matrix[i][j].red = image->matrix[image->rows - 1 - j][i].red;
			aux_matrix[image->rows - 1 - j][i].red = image->matrix[image->rows - 1 - i][image->rows - 1 - j].red;
			aux_matrix[image->rows - 1 - i][image->rows - 1 - j].red = image->matrix[j][image->rows - 1 - i].red;
			aux_matrix[j][image->rows - 1 - i].red = aux;
		}
	}
	for(int i = 0; i < image->rows; i++)
		for(int j = 0; j < image->cols; j++)
			image->matrix[i][j].red = aux_matrix[i][j].red;
	free_matrix_rgb(aux_matrix, image->rows);
}
void rotate_90_gray_left(image *img) {
	rgb **aux_matrix = malloc(img->rows * sizeof(rgb *));
	for(int i = 0; i < img->rows; i++)
		aux_matrix[i] = malloc(img->cols * sizeof(rgb));
	for(int i = 0; i < img->rows; i++)
		for(int j = 0; j < img->cols; j++)
			aux_matrix[i][j].red = img->matrix[i][j].red;
	for (int i = 0; i < img->rows / 2; i++)
	{
		for (int j = i; j < img->rows - i - 1; j++)
		{
			int aux;
			aux= img->matrix[i][j].red;
			aux_matrix[i][j].red = img->matrix[j][img->rows - 1 - i].red;
            aux_matrix[j][img->rows - 1 - i].red = img->matrix[img->rows - 1 - i][img->rows - 1 - j].red;
            aux_matrix[ img->rows- 1 - i][img->rows - 1 - j].red = img->matrix[img->rows - 1 - j][i].red;
            aux_matrix[img->rows - 1 - j][i].red = aux;
		}
	}
	for(int i = 0; i < img->rows; i++)
		for(int j = 0; j < img->cols; j++)
			img->matrix[i][j].red = aux_matrix[i][j].red;
	free_matrix_rgb(aux_matrix, img->rows);
}
void rotate90_rgb_right(image *image)
{
	rgb **aux_matrix = malloc(image->rows * sizeof(rgb *));
	for(int i = 0; i < image->rows; i++)
		aux_matrix[i] = malloc(image->cols * sizeof(rgb));
	for(int i = 0; i < image->rows; i++){
		for(int j = 0; j < image->cols; j++){
			aux_matrix[i][j].red = image->matrix[i][j].red;
			aux_matrix[i][j].green = image->matrix[i][j].green;
			aux_matrix[i][j].blue = image->matrix[i][j].blue;
		}
	}
	for (int i = 0; i < image->rows / 2; i++)
	{
		for (int j = i; j < image->rows - i - 1; j++)
		{
			int aux;
			aux = image->matrix[i][j].red;
			aux_matrix[i][j].red = image->matrix[image->rows - 1 - j][i].red;
			aux_matrix[image->rows - 1 - j][i].red = image->matrix[image->rows - 1 - i][image->rows - 1 - j].red;
			aux_matrix[image->rows - 1 - i][image->rows - 1 - j].red = image->matrix[j][image->rows - 1 - i].red;
			aux_matrix[j][image->rows - 1 - i].red = aux;

			int aux1;
			aux1 = image->matrix[i][j].green;
			aux_matrix[i][j].green = image->matrix[image->rows - 1 - j][i].green;
			aux_matrix[image->rows - 1 - j][i].green = image->matrix[image->rows - 1 - i][image->rows - 1 - j].green;
			aux_matrix[image->rows - 1 - i][image->rows - 1 - j].green = image->matrix[j][image->rows - 1 - i].green;
			aux_matrix[j][image->rows - 1 - i].green = aux1;

			int aux2;
			aux2 = image->matrix[i][j].blue;
			aux_matrix[i][j].blue = image->matrix[image->rows - 1 - j][i].blue;
			aux_matrix[image->rows - 1 - j][i].blue = image->matrix[image->rows - 1 - i][image->rows - 1 - j].blue;
			aux_matrix[image->rows - 1 - i][image->rows - 1 - j].blue = image->matrix[j][image->rows - 1 - i].blue;
			aux_matrix[j][image->rows - 1 - i].blue = aux2;
		}
	}
	for(int i = 0; i < image->rows; i++){
		for(int j = 0; j < image->cols; j++){
			image->matrix[i][j].red = aux_matrix[i][j].red;
			image->matrix[i][j].green = aux_matrix[i][j].green;
			image->matrix[i][j].blue = aux_matrix[i][j].blue;
		}
		}
	free_matrix_rgb(aux_matrix, image->rows);
}
void rotate_rgb_90_left(image *img) {
	rgb **aux_matrix = malloc(img->rows * sizeof(rgb *));
	for(int i = 0; i < img->rows; i++)
		aux_matrix[i] = malloc(img->cols * sizeof(rgb));
	for(int i = 0; i < img->rows; i++){
		for(int j = 0; j < img->cols; j++){
			aux_matrix[i][j].red = img->matrix[i][j].red;
			aux_matrix[i][j].green = img->matrix[i][j].green;
			aux_matrix[i][j].blue = img->matrix[i][j].blue;
		}
	}
	for (int i = 0; i < img->rows / 2; i++)
	{
		for (int j = i; j < img->rows - i - 1; j++)
		{
			int aux;
			aux=img->matrix[i][j].red;
			aux_matrix[i][j].red = img->matrix[j][img->rows - 1 - i].red;
            aux_matrix[j][img->rows - 1 - i].red = img->matrix[img->rows - 1 - i][img->rows - 1 - j].red;
            aux_matrix[ img->rows- 1 - i][img->rows - 1 - j].red = img->matrix[img->rows - 1 - j][i].red;
            aux_matrix[img->rows - 1 - j][i].red = aux;
			
			int aux1;
			aux1=img->matrix[i][j].green;
			aux_matrix[i][j].green = img->matrix[j][img->rows - 1 - i].green;
            aux_matrix[j][img->rows - 1 - i].green = img->matrix[img->rows - 1 - i][img->rows - 1 - j].green;
            aux_matrix[ img->rows- 1 - i][img->rows - 1 - j].green = img->matrix[img->rows - 1 - j][i].green;
            aux_matrix[img->rows - 1 - j][i].green = aux1;
			
			int aux2;
			aux2=img->matrix[i][j].blue;
			aux_matrix[i][j].blue = img->matrix[j][img->rows - 1 - i].blue;
            aux_matrix[j][img->rows - 1 - i].blue = img->matrix[img->rows - 1 - i][img->rows - 1 - j].blue;
            aux_matrix[ img->rows- 1 - i][img->rows - 1 - j].blue = img->matrix[img->rows - 1 - j][i].blue;
            aux_matrix[img->rows - 1 - j][i].blue = aux2;
		}
	}
	for(int i = 0; i < img->rows; i++){
		for(int j = 0; j < img->cols; j++){
			img->matrix[i][j].red = aux_matrix[i][j].red;
			img->matrix[i][j].green = aux_matrix[i][j].green;
			img->matrix[i][j].blue = aux_matrix[i][j].blue;
		}
	}

	free_matrix_rgb(aux_matrix, img->rows);
}

void rotate90_gray_right_select(image *image)
{
	rgb **aux_matrix = malloc((image->select_y2-image->select_y1) * sizeof(rgb *));
	for(int i = 0; i < image->select_y2-image->select_y1; i++)
		aux_matrix[i] = malloc((image->select_x2-image->select_x1) * sizeof(rgb));
	for(int i = 0; i < image->select_y2-image->select_y1; i++)
		for(int j = 0; j < image->select_x2-image->select_x1; j++)
			aux_matrix[i][j].red = image->matrix[i][j].red;
	for (int i = 0; i < (image->select_y2-image->select_y1) / 2; i++)
	{
		for (int j = i; j < image->select_x2-image->select_x1 - i - 1; j++)
		{
			int aux;
			aux = image->matrix[i][j].red;
			aux_matrix[i][j].red = image->matrix[(image->select_x2-image->select_x1) - 1 - j][i].red;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - j][i].red = 
			image->matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].red;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].red = 
			image->matrix[j][(image->select_x2-image->select_x1) - 1 - i].red;
			aux_matrix[j][(image->select_x2-image->select_x1) - 1 - i].red = aux;
		}
	}
	for(int i = 0; i < (image->select_x2-image->select_x1); i++)
		for(int j = 0; j < (image->select_x2-image->select_x1); j++)
			image->matrix[i][j].red = aux_matrix[i][j].red;
	free_matrix_rgb(aux_matrix, (image->select_y2-image->select_y1) );
}
void rotate_90_gray_left_select(image *img) {
	rgb **aux_matrix = malloc((img->select_x2-img->select_x1) * sizeof(rgb *));
	for(int i = 0; i < (img->select_x2-img->select_x1); i++)
		aux_matrix[i] = malloc((img->select_x2-img->select_x1) * sizeof(rgb));
	for(int i = 0; i < (img->select_x2-img->select_x1); i++)
		for(int j = 0; j < (img->select_x2-img->select_x1); j++)
			aux_matrix[i][j].red = img->matrix[i][j].red;
	for (int i = 0; i < (img->select_x2-img->select_x1) / 2; i++)
	{
		for (int j = i; j <(img->select_x2-img->select_x1)- i - 1; j++)
		{
			int aux;
			aux= img->matrix[i][j].red;
			aux_matrix[i][j].red = img->matrix[j][(img->select_x2-img->select_x1) - 1 - i].red;
            aux_matrix[j][(img->select_x2-img->select_x1) - 1 - i].red = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - i][(img->select_x2-img->select_x1) - 1 - j].red;
            aux_matrix[ (img->select_x2-img->select_x1)- 1 - i][(img->select_x2-img->select_x1) - 1 - j].red = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - j][i].red;
            aux_matrix[(img->select_x2-img->select_x1) - 1 - j][i].red = aux;
		}
	}
	for(int i = 0; i <(img->select_x2-img->select_x1); i++)
		for(int j = 0; j < (img->select_x2-img->select_x1); j++)
			img->matrix[i][j].red = aux_matrix[i][j].red;
	free_matrix_rgb(aux_matrix, (img->select_x2-img->select_x1));
}
void rotate90_rgb_right_select(image *image)
{
	rgb **aux_matrix = malloc((image->select_y2-image->select_y1) * sizeof(rgb *));
	for(int i = 0; i < image->select_y2-image->select_y1; i++)
		aux_matrix[i] = malloc((image->select_x2-image->select_x1) * sizeof(rgb));
	for(int i = 0; i < image->select_y2-image->select_y1; i++){
		for(int j = 0; j < image->select_x2-image->select_x1; j++){
			aux_matrix[i][j].red= image->matrix[i][j].red;
			aux_matrix[i][j].green= image->matrix[i][j].green;
			aux_matrix[i][j].blue= image->matrix[i][j].blue;
		}
	}
	for (int i = 0; i < (image->select_y2-image->select_y1) / 2; i++)
	{
		for (int j = i; j < image->select_x2-image->select_x1 - i - 1; j++)
		{
			int aux;
			aux = image->matrix[i][j].red;
			aux_matrix[i][j].red = image->matrix[(image->select_x2-image->select_x1) - 1 - j][i].red;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - j][i].red = 
			image->matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].red;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].red = 
			image->matrix[j][(image->select_x2-image->select_x1) - 1 - i].red;
			aux_matrix[j][(image->select_x2-image->select_x1) - 1 - i].red = aux;

			int aux1;
			aux1 = image->matrix[i][j].green;
			aux_matrix[i][j].green = image->matrix[(image->select_x2-image->select_x1) - 1 - j][i].green;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - j][i].green = 
			image->matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].green;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].green = 
			image->matrix[j][(image->select_x2-image->select_x1) - 1 - i].green;
			aux_matrix[j][(image->select_x2-image->select_x1) - 1 - i].green = aux1;

			int aux2;
			aux2 = image->matrix[i][j].blue;
			aux_matrix[i][j].blue = image->matrix[(image->select_x2-image->select_x1) - 1 - j][i].blue;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - j][i].blue = 
			image->matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].blue;
			aux_matrix[(image->select_x2-image->select_x1) - 1 - i][(image->select_x2-image->select_x1) - 1 - j].blue = 
			image->matrix[j][(image->select_x2-image->select_x1) - 1 - i].blue;
			aux_matrix[j][(image->select_x2-image->select_x1) - 1 - i].blue = aux2;
		}
		}
		

	
	for(int i = 0; i < (image->select_x2-image->select_x1); i++){
		for(int j = 0; j < (image->select_x2-image->select_x1); j++){
			image->matrix[i][j].red = aux_matrix[i][j].red;
			image->matrix[i][j].green = aux_matrix[i][j].green;
			image->matrix[i][j].blue = aux_matrix[i][j].blue;
	
		}
	}
	free_matrix_rgb(aux_matrix, (image->select_x2-image->select_x1));
}
void rotate_90_rgb_left_select(image *img) {
	rgb **aux_matrix = malloc((img->select_x2-img->select_x1) * sizeof(rgb *));
	for(int i = 0; i < (img->select_x2-img->select_x1); i++)
		aux_matrix[i] = malloc((img->select_x2-img->select_x1) * sizeof(rgb));
	for(int i = 0; i < (img->select_x2-img->select_x1); i++){
		for(int j = 0; j < (img->select_x2-img->select_x1); j++){
			aux_matrix[i][j].red = img->matrix[i][j].red;
			aux_matrix[i][j].green = img->matrix[i][j].green;
			aux_matrix[i][j].blue = img->matrix[i][j].blue;
		}
	}
	for (int i = 0; i < (img->select_x2-img->select_x1) / 2; i++)
	{
		for (int j = i; j <(img->select_x2-img->select_x1)- i - 1; j++)
		{
			int aux;
			aux= img->matrix[i][j].red;
			aux_matrix[i][j].red = img->matrix[j][(img->select_x2-img->select_x1) - 1 - i].red;
            aux_matrix[j][(img->select_x2-img->select_x1) - 1 - i].red = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - i][(img->select_x2-img->select_x1) - 1 - j].red;
            aux_matrix[ (img->select_x2-img->select_x1)- 1 - i][(img->select_x2-img->select_x1) - 1 - j].red = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - j][i].red;
            aux_matrix[(img->select_x2-img->select_x1) - 1 - j][i].red = aux;

			int aux1;
			aux1= img->matrix[i][j].green;
			aux_matrix[i][j].green = img->matrix[j][(img->select_x2-img->select_x1) - 1 - i].green;
            aux_matrix[j][(img->select_x2-img->select_x1) - 1 - i].green = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - i][(img->select_x2-img->select_x1) - 1 - j].green;
            aux_matrix[ (img->select_x2-img->select_x1)- 1 - i][(img->select_x2-img->select_x1) - 1 - j].green = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - j][i].green;
            aux_matrix[(img->select_x2-img->select_x1) - 1 - j][i].green = aux1;

			int aux2;
			aux2= img->matrix[i][j].blue;
			aux_matrix[i][j].blue = img->matrix[j][(img->select_x2-img->select_x1) - 1 - i].blue;
            aux_matrix[j][(img->select_x2-img->select_x1) - 1 - i].blue = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - i][(img->select_x2-img->select_x1) - 1 - j].blue;
            aux_matrix[ (img->select_x2-img->select_x1)- 1 - i][(img->select_x2-img->select_x1) - 1 - j].blue = 
			img->matrix[(img->select_x2-img->select_x1) - 1 - j][i].blue;
            aux_matrix[(img->select_x2-img->select_x1) - 1 - j][i].blue = aux2;
		}
	}
	for(int i = 0; i <(img->select_x2-img->select_x1); i++){
		for(int j = 0; j < (img->select_x2-img->select_x1); j++){
			img->matrix[i][j].red = aux_matrix[i][j].red;
			img->matrix[i][j].green = aux_matrix[i][j].green;
			img->matrix[i][j].blue = aux_matrix[i][j].blue;
		}
	}
		free_matrix_rgb(aux_matrix, (img->select_x2-img->select_x1));
}
void rotate(image *img,char **words){
	
	int angle=atoi(words[1]);
	
	if(img->select_x2-img->select_x1 != img->select_y2-img->select_y1){
		printf("Selected area must be square\n");
		return;
	}

	if(img->select_x1==0 && img->select_x2==0 && img->select_y1==0 && img->select_y2==0){
		img->select_x1=0;
		img->select_x2=img->cols;
		img->select_y1=0;
		img->select_y2=img->rows;
	}
	if(angle==0) {
	printf("Rotated 0\n");
	return;
	}
	if(angle==360){
		printf("Rotated 360\n");
		return;
	}
	if(angle==-360){
		printf("Rotated -360\n");
		return;
	}
	if(img->select_x1==0&&img->select_x2==img->cols&&img->select_y1==0&&img->select_y2==img->rows){
		if(strcmp(img->type,"P2")==0 || strcmp(img->type,"P5")==0){
			if(angle==90){
				rotate90_gray_right(img);
				printf("Rotated 90\n");
				return;
			}
			if(angle==-90){
				rotate_90_gray_left(img);
				printf("Rotated -90\n");
				return;
			}
			if(angle==180){
				rotate90_gray_right(img);
				rotate90_gray_right(img);
				printf("Rotated 180\n");
				return;
			}
			if(angle==-180){
				rotate90_gray_right(img);
				rotate90_gray_right(img);
				printf("Rotated -180\n");
				return;
			}
			if(angle==270){
				rotate90_gray_right(img);
				rotate90_gray_right(img);
				rotate90_gray_right(img);
				printf("Rotated 270\n");
				return;
			}
			if(angle==-270){
				rotate_90_gray_left(img);
				rotate_90_gray_left(img);
				rotate_90_gray_left(img);
				printf("Rotated -270\n");
				return;
			}

			

		}
		if(strcmp(img->type,"P3")==0 || strcmp(img->type,"P6")==0){
			if(angle==90){
				rotate90_rgb_right(img);
				printf("Rotated 90\n");
				return;
			}
			if(angle==-90){
				rotate_rgb_90_left(img);
				printf("Rotated -90\n");
				return;
			}
			if(angle==180){
				rotate90_rgb_right(img);
				rotate90_rgb_right(img);
				printf("Rotated 180\n");
				return;
			}
			if(angle==-180){
				rotate90_rgb_right(img);
				rotate90_rgb_right(img);
				printf("Rotated -180\n");
				return;
			}
			if(angle==270){
				rotate90_rgb_right(img);
				rotate90_rgb_right(img);
				rotate90_rgb_right(img);
				printf("Rotated 270\n");
				return;
			}
			if(angle==-270){
				rotate_rgb_90_left(img);
				rotate_rgb_90_left(img);
				rotate_rgb_90_left(img);
				printf("Rotated -270\n");
				return;
			}
		}
	}
	
	if(strcmp(img->type,"P2")==0 || strcmp(img->type,"P5")==0){
		
		if(angle==90){
				rotate90_gray_right_select(img);
				printf("Rotated 90\n");
				return;
			}
			if(angle==-90){
				rotate_90_gray_left_select(img);
				printf("Rotated -90\n");
				return;
			}
			if(angle==180){
				rotate90_gray_right_select(img);
				rotate90_gray_right_select(img);
				printf("Rotated 180\n");
				return;
			}
			if(angle==-180){
				rotate90_gray_right_select(img);
				rotate90_gray_right_select(img);
				printf("Rotated -180\n");
				return;
			}
			if(angle==270){
				rotate90_gray_right_select(img);
				rotate90_gray_right_select(img);
				rotate90_gray_right_select(img);
				printf("Rotated 270\n");
				return;
			}
			if(angle==-270){
				rotate_90_gray_left_select(img);
				rotate_90_gray_left_select(img);
				rotate_90_gray_left_select(img);
				printf("Rotated -270\n");
				return;
			}
		}
		if(strcmp(img->type,"P3")==0 || strcmp(img->type,"P6")==0){
			if(angle==90){
				rotate90_rgb_right_select(img);
				printf("Rotated 90\n");
				return;
			}
			if(angle==-90){
				rotate_90_rgb_left_select(img);
				printf("Rotated -90\n");
				return;
			}
			if(angle==180){
				rotate90_rgb_right_select(img);
				rotate90_rgb_right_select(img);
				printf("Rotated 180\n");
				return;
			}
			if(angle==-180){
				rotate90_rgb_right_select(img);
				rotate90_rgb_right_select(img);
				printf("Rotated -180\n");
				return;
			}
			if(angle==270){
				rotate90_rgb_right_select(img);
				rotate90_rgb_right_select(img);
				rotate90_rgb_right_select(img);
				printf("Rotated 270\n");
				return;
			}
			if(angle==-270){
				rotate_90_rgb_left_select(img);
				rotate_90_rgb_left_select(img);
				rotate_90_rgb_left_select(img);
				printf("Rotated -270\n");
				return;
			}
		}
	}

