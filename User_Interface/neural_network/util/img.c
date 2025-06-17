#include "img.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdint.h>
#include <math.h>
#include <dirent.h>
#include <unistd.h>
#include "../neural/nn.h"
#define MAXCHAR 10000

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface_temp = IMG_Load(path);
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(surface_temp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface_temp);
    return surface;

}


void directory_to_solver() 
{
	int file_solver = open("grid_00",O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	NeuralNetwork* net = network_load("testing_net");
	int count = 0;
	int saut = 0;
	for (int premiereVariable = 0; premiereVariable < 9; premiereVariable++)
	{
		for (int deuxiemeVariable = 0; deuxiemeVariable < 9; deuxiemeVariable++)
		{
			if (count != 0 && (count)%3==0)
			{
				if ((count)%9==0)
				{
					saut+=1;
					if(saut!=0 && saut%3==0)
					{
						write(file_solver,"\n",1);
					}
					write(file_solver,"\n",1);
				}
				else
				{
					write(file_solver," ",1);
				}
			}
			const char *prefixe = "../image_test_nn/";
			size_t totalLength = strlen(prefixe) +
								snprintf(NULL, 0, "%d", premiereVariable) +
								snprintf(NULL, 0, "%d", deuxiemeVariable) +
								strlen(".bmp") + 1;
			char *resultat = (char *)malloc(totalLength);
			sprintf(resultat, "%s%d%d.bmp", prefixe, premiereVariable, deuxiemeVariable);
			Img* img = image_character_to_img(resultat,'b');
			Matrix* res = network_predict_img(net, img);
			int n_predicted = matrix_argmax(res);
			if (n_predicted==0)
			{
				write(file_solver,".",1);
			}
			else
			{
				char caractere = n_predicted + '0';
				write(file_solver, &caractere, 1);
			}	
			free(resultat);
			count+=1;

		}
		
	}
	close(file_solver);
}


Img* image_character_to_img(const char* file_path,char color)
{
	Img* img = malloc(sizeof(Img*));
	SDL_Surface* surface = load_image(file_path);
	img->img_data = matrix_create(28, 28);
	Uint32* pixels = surface->pixels;
	int width = surface->w ;
  	int height = surface->h;
	SDL_PixelFormat* format = surface->format;
	SDL_LockSurface(surface);
  	int delta_width = (width % 28)/2 ; 
	int delta_height = (height% 28)/2 ;
	int h_28 = (height)/28;
	int w_28 = (width)/28;
	for(int i = 0;i<28;i++)
	{
    	for(int j = 0;j<28;j++)
	  	{
			double tot = 0;
			double count = 0;
			for (int k = 0; k < h_28; k++)
			{
				for (int m = 0; m < w_28; m++)
				{
					Uint8 r, g, b;
					SDL_GetRGB(pixels[(((i*h_28+k+delta_height)*width))+j*w_28+m+delta_width], format, &r, &g, &b);
					
					if(r < 128)
					{
						tot+= r;
						count+=1;
					}
					else
					{
						tot += r;
						count += 1;
					}
				}
			}
			double moy = (tot / count) / 255;
			if (color == 'b')
			{
				if (moy > 0.5)
				{
					img->img_data->entries[i][j] = 0;
				}
				else
				{
					img->img_data->entries[i][j]= 1 -( (tot / count) / 256);
				}
			}
			else
			{
				if (moy > 0.5)
				{
					img->img_data->entries[i][j]=(tot / count) / 256;
				}
				else
				{
					img->img_data->entries[i][j] = 0;
				}
			}
	  	}
	}
	SDL_UnlockSurface(surface);
	return img;	 	
}


Img** load_images_from_directory(char* directory_path, int number_of_imgs) 
{
    
    Img** imgs = malloc(number_of_imgs * sizeof(Img*)*800);
	long count = 0;
	int la =0;
	for (size_t i = 0; i < 1; i++)
	{
		DIR *dossier;
		struct dirent *entree;
		dossier = opendir(directory_path);
		if (dossier == NULL) {
			perror("Unable to open the directory");
			exit(EXIT_FAILURE);
		}
		while ((entree = readdir(dossier)) != NULL && count < number_of_imgs) 
		{
			if (strcmp(entree->d_name, ".") != 0 && strcmp(entree->d_name, "..") != 0) 
			{
				char* str = directory_path;
				char* str_2 = (char*)malloc(strlen(str) + 2);
				strcpy(str_2, str);
				strcat(str_2, "/");
				char* nouvelle_chaine = (char*)malloc(strlen(str_2) + strlen(entree->d_name) + 1);
				strcpy(nouvelle_chaine, str_2);
				strcat(nouvelle_chaine, entree->d_name);
				int label = (int)entree->d_name[0]-48;
				Img* img = image_character_to_img(nouvelle_chaine,'w');
				img->label = label;
				imgs[count] = img;
				count += 1;
				free(nouvelle_chaine);
				free(str_2);
			}
		}
		closedir(dossier);
	}
	for (size_t i = 0; i < 40; i++)
	{
		DIR *dossier;
		struct dirent *entree;
		dossier = opendir(directory_path);
		if (dossier == NULL) {
			perror("Unable to open the directory");
			exit(EXIT_FAILURE);
		}
		while ((entree = readdir(dossier)) != NULL && count < number_of_imgs) 
		{
			if (strcmp(entree->d_name, ".") != 0 && strcmp(entree->d_name, "..") != 0) 
			{
				char* str = directory_path;
				char* str_2 = (char*)malloc(strlen(str) + 2);
				strcpy(str_2, str);
				strcat(str_2, "/");
				char* nouvelle_chaine = (char*)malloc(strlen(str_2) + strlen(entree->d_name) + 1);
				strcpy(nouvelle_chaine, str_2);
				strcat(nouvelle_chaine, entree->d_name);
				int label = (int)entree->d_name[0]-48;
				Img* img = image_character_to_img(nouvelle_chaine,'b');
				img->label = label;
				imgs[count] = img;
				count += 1;
				free(nouvelle_chaine);
				free(str_2);
			}
		}
		closedir(dossier);
	}
    return imgs;
}



void img_print(Img* img) 
{
	matrix_print(img->img_data);
	printf("Img Label: %d\n", img->label);
}

void img_free(Img* img) 
{
	matrix_free(img->img_data);
	free(img);
	img = NULL;
}

void imgs_free(Img** imgs, int n) 
{
	for (int i = 0; i < n; i++) 
	{
		img_free(imgs[i]);
	}
	free(imgs);
	imgs = NULL;
}
