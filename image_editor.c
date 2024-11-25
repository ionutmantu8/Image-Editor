#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "header.h"
//Copyright Ionut Mantu 313 CAb 2023-2024
int main(void)
{
	image *newImage = NULL;
	while (1)
	{
		char com[1000];
		fgets(com, 999, stdin);
		char *words[100];
		words[0] = strtok(com, " \n");
		int i = 1;
		while ((words[i] = strtok(NULL, " \n")))
		{
			i++;
		}

		int x = command(words[0], words[1]);
		switch (x)
		{
		case 0:
			freeimage(newImage);
			newImage = load(words[1]);
			if (newImage == NULL)
			{
				printf("Failed to load %s\n", words[1]);
			}
			break;

		case 1:
			if (newImage == NULL)
			{
				printf("No image loaded\n");
				break;
			}
			save(words[1], newImage, words[2]);
			break;
		case 2:
			crop(newImage);
			break;
		case 3:
			exit_program(newImage);
			exit(0);
			break;
		case 4:
			select_all(newImage);
			break;
		case 5:
			verify_histo(newImage, words);
			break;
		case 6:
			if((i - 1) != 4){
				printf("Invalid command\n");
				break;
					}
			select_area(newImage,words);
			break;
		case 7:
			if(newImage==NULL){
		printf("No image loaded\n");
		break;
		}
		if(words[1]==NULL){
			printf("Invalid command\n");
			break;
		}
		if(atoi(words[1])%90!=0){
			printf("Unsupported rotation angle\n");
			break;
			}
		if(newImage->select_x2-newImage->select_x1 != newImage->select_y2-newImage->select_y1){
		printf("Selected area must be square\n");
		break;
		}
			rotate(newImage,words);
			break;
		case 8:
			apply(newImage,words[1]);
			break;
		case 9:
			equalize(newImage);
			break;
		default:
			printf("Invalid command\n");
			break;
		}
	}

	return 0;
}
