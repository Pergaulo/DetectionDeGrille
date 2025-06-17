#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define M_PI 3.14159265358979323846
// Prototype de la fonction

Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
	//recupere les pixels
	int bpp = surf->format->BytesPerPixel;
	return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{	
	Uint8 *p = pixel_ref(surface, x, y);
	

	switch (surface->format->BytesPerPixel)
	{
		case 1:
			return *p;

		case 2:
			return *(Uint16 *)p;

		case 3:
			if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		case 4:
			return *(Uint32 *)p;
	}

	return 0;
}

int **separerListes(int *liste, int taille, int *tailleSousListes) {
    int **sousListes = malloc(2 * sizeof(int*));
    int *tailles = malloc(taille * sizeof(int));
    int **tempListes = malloc(taille * sizeof(int*));
    int indexSousListe = -1;
    int tailleSousListe = 0;

    for (int i = 0; i < taille; i++) {
        if (i == 0 || (liste[i] - liste[i - 1] > 2)) {
            tailleSousListe = 1;
            indexSousListe++;
            tempListes[indexSousListe] = malloc(taille * sizeof(int));
            tempListes[indexSousListe][0] = liste[i];
        } else {
            tempListes[indexSousListe][tailleSousListe] = liste[i];
            tailleSousListe++;
        }
        tailles[indexSousListe] = tailleSousListe;
    }

    int max1 = 0, max2 = 0, maxIndex1 = -1, maxIndex2 = -1;
    for (int i = 0; i <= indexSousListe; i++) {
        if (tailles[i] > max1) {
            max2 = max1;
            maxIndex2 = maxIndex1;
            max1 = tailles[i];
            maxIndex1 = i;
        } else if (tailles[i] > max2) {
            max2 = tailles[i];
            maxIndex2 = i;
        }
    }

    sousListes[0] = malloc(max1 * sizeof(int));
    memcpy(sousListes[0], tempListes[maxIndex1], max1 * sizeof(int));
    tailleSousListes[0] = max1;

    sousListes[1] = malloc(max2 * sizeof(int));
    memcpy(sousListes[1], tempListes[maxIndex2], max2 * sizeof(int));
    tailleSousListes[1] = max2;

    // Libérer la mémoire temporaire
    for (int i = 0; i <= indexSousListe; i++) {
        free(tempListes[i]);
    }
    free(tempListes);
    free(tailles);

    return sousListes;
}


double calculerMoyenne(int *liste, int taille) {
    if (taille == 0) {
        return 0.0;
    }

    double somme = 0.0;

    for (int i = 0; i < taille; i++) {
        somme += liste[i];
    }

    return somme / taille;
}

int* hough_rotation(SDL_Surface *surface, int x, int y){
	//On commence par définir l'accumulateur. rho et theta sont les résolutions, respectivement en pixel et en degré.
	int Ny = y;
	int Nx = x;
	double rho = 1.0;
	double theta = 1.0;
	int Ntheta = (int)(180.0/theta);
	int Nrho = (int)((sqrt(Nx * Nx + Ny * Ny) / rho));
	double dtheta = M_PI/Ntheta;
	//printf("laaaaaaaaa : %f\n",dtheta);
	double drho = (sqrt(Nx * Nx + Ny * Ny) / Nrho);
	int accum[Ntheta][Nrho];
	// Initialiser le tableau avec des zéros
	for (int i = 0; i < Ntheta; i++) {
		for (int j = 0; j < Nrho; j++) {
		     accum[i][j] = 0;
		     }
	}
	

	//Pour chaque pixel noir de l'image, on incrémente les points de l'accumulateur obtenus en échantillonnant la sinusoïde : 
	for (int j = 0; j < Ny; j++) {
		    for (int i = 0; i < Nx; i++) {
		   		Uint32 pixel = get_pixel(surface, i, j);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, surface->format, &r, &g, &b);
                    if (r == 0 && g == 0 && b == 0) 
		            for (int i_theta = 0; i_theta < Ntheta; i_theta++) {
		                double theta = i_theta * dtheta;
		                double rho = i * cos(theta) + (Ny - j) * sin(theta);
		                int i_rho = (int)(rho / drho);
		                if (i_rho > 0 && i_rho < Nrho) {
		                    accum[i_theta][i_rho]++;
		                }
		            }
		        }
		    }
	    
	//On retient les valeurs dans l'accumulateur au dessus d'un seuil :
	int seuil = 500;
	int accum_seuil[Ntheta][Nrho];
	for (int i_theta = 0; i_theta < Ntheta; i_theta++){
		for (int i_rho = 0; i_rho < Nrho; i_rho++){
		        accum_seuil[i_theta][i_rho] = accum[i_theta][i_rho];
		}
	}
	
	for (int i_theta = 0; i_theta < Ntheta; i_theta++){
		for (int i_rho = 0; i_rho < Nrho; i_rho++){
		    if (accum[i_theta][i_rho] < seuil)
		        accum_seuil[i_theta][i_rho] = 0;
		}
	}
	
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
			
		    if (accum_seuil[i_theta][i_rho] != 0)
		        {
					int dist_theta = 60;
					int dist_rho = 60;
					for(int i = -dist_theta; i < dist_theta; i++)
					{
						for (int y = - dist_rho; y < dist_rho; y++)
						{
							if(i_theta + i >= 0 && i_theta + i < Ntheta && i_rho + y >= 0 && i_rho + y < Nrho)
							{
								if (accum_seuil[i_theta + i][i_rho + y] > accum_seuil[i_theta][i_rho])
								{
									accum_seuil[i_theta][i_rho] = 0;
								}
							}
						}
					}
					
				}
		}
	}
	

	int ind_1 = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
			
		    if (accum_seuil[i_theta][i_rho] != 0)
		    {
				ind_1++;
				//printf("f");
			}

		}
	}

	int l[ind_1][3];
	int countt = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		int temp_seuil = accum_seuil[i_theta][0];
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
		    if (accum_seuil[i_theta][i_rho] != 0)
		    {
				l[countt][0] = i_theta; 
				l[countt][1] = i_rho; 
				l[countt][2] = accum_seuil[i_theta][i_rho]; 
				countt ++;
			}
		}
	}  
	//On constate qu'il y a une forte accumulation autour de certains points, alors que d'autres sont à peine visibles. Pour bien voir les droites obtenues, nous allons placer les couples (ρ,θ) dans une liste : 
	
	size_t count = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
		    if (accum_seuil[i_theta][i_rho] != 0)
		    	{
		    		count++;
		    	}
		}
	}
	
	int* list_theta = malloc((count + 1)* sizeof(int));
	list_theta[0] = count;
	size_t i = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++){
		for (int i_rho = 0; i_rho < Nrho; i_rho++){
		    if (accum_seuil[i_theta][i_rho] != 0)
		    	{
					list_theta[i + 1] = i_theta;
		    		i++;
		    	}
		}
	}
	return list_theta;
}




// Add a border on the image
SDL_Surface* enlargeImageWithRedBackground(SDL_Surface* image, int borderSize) 
{
    if (image == NULL || borderSize <= 0) 
    {
	    return NULL;
    }

    int oldWidth = image->w;
    int oldHeight = image->h;

    // Calculate the new dimensions of the image after enlargements
    int newWidth = oldWidth + 3 * borderSize;
    int newHeight = oldHeight + 3 * borderSize;

    // Create a empty surface for the new image with border
    SDL_Surface* enlargedImage = SDL_CreateRGBSurface(0, newWidth, newHeight, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

    if (enlargedImage != NULL) 
    {
	    // Fill the new surface with a red color
	    Uint32 red = SDL_MapRGB(enlargedImage->format, 100, 0, 0);
	    SDL_FillRect(enlargedImage, NULL, red);
	    
	    // put the image in the center of the new red surface
	    int offsetX = borderSize + 350;
	    int offsetY = borderSize + 350;
	    
	    for (int x = 0; x < oldWidth; x++) 
	    {
		    for (int y = 0; y < oldHeight; y++)
		    {
			    Uint32 pixel = ((Uint32*)image->pixels)[x + y * oldWidth];
			    ((Uint32*)enlargedImage->pixels)[x + offsetX + (y + offsetY) * newWidth] = pixel;
		    }
	    }
    }
    return enlargedImage;
}




// Apply a rotation on the image with angle
SDL_Surface* rotateImage(SDL_Surface* image, double angle) 
{
    if (image == NULL) 
    {
	    return NULL;
    }

    SDL_Surface* rotatedImage = NULL;

    // Calculate the new dimensions of image after rotation
    int newWidth = image->w;
    int newHeight = image->h;
    double radAngle = angle * M_PI / 180.0;
    double cosine = cos(radAngle);
    double sine = sin(radAngle);

    newWidth = fabs(image->w * cosine) + fabs(image->h * sine);
    newHeight = fabs(image->w * sine) + fabs(image->h * cosine);

    // Create a new empty surface for rotative image
	
    rotatedImage = SDL_CreateRGBSurface(0, newWidth, newHeight, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask);

    if (rotatedImage != NULL) 
    {
		SDL_FillRect(rotatedImage, NULL, SDL_MapRGBA(rotatedImage->format, 255, 255, 255, 255));

	    // Rotation of the image
	    SDL_SetSurfaceRLE(image, 1);
	    SDL_SetSurfaceRLE(rotatedImage, 1);
	    double cx = newWidth/3;
	    double cy = newHeight/3;
	    for (int x = 0; x < newWidth; x++) 
	    {
		    for (int y = 0; y < newHeight; y++) 
		    {
			    double originX = cosine * (x - cx) + sine * (y - cy) + cx;
			    double originY = -sine * (x - cx) + cosine * (y - cy) + cy;
			    if (originX >= 0 && originX < image->w && originY >= 0 && originY < image->h) 
			    {
				    Uint32 pixel = ((Uint32*)image->pixels)[(int)originX + (int)originY * image->w];
				    ((Uint32*)rotatedImage->pixels)[x + y * rotatedImage->w] = pixel;
			    }
		    }
	    }
    }
    return rotatedImage;
}




//Load image
SDL_Surface* load_image(const char* path)
{
	SDL_Surface* surface_t = IMG_Load(path);
    	SDL_Surface* surface = SDL_ConvertSurfaceFormat(surface_t, SDL_PIXELFORMAT_RGB888, 0);
    	SDL_FreeSurface(surface_t);
    	return surface;
}




//Main 
int main(int argc, char** argv[]) {
	
	//Check argument
	if (argc != 2)
		errx(EXIT_FAILURE, "error : need 3 arguments");
        
	// Init SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "Erreur d'initialisation de SDL : %s\n", SDL_GetError());
		return 1;
	}
	
	// Load image
	SDL_Surface* image = load_image(argv[1]);
	if (image == NULL) 
	{
		fprintf(stderr, "Erreur de chargement de l'image : %s\n", IMG_GetError());
		return 1;
	}

	int* teta = hough_rotation(image, image->w, image->h);

    int taille = teta[0];
    int *liste = malloc(taille * sizeof(int));

    for (int i = 0; i < taille; i++) {
        liste[i] = teta[i + 1];
		//printf("%d\n", liste[i]);
    }

    int taillesSousListes[2];
    int **sousListes = separerListes(liste, taille, taillesSousListes);

    double moyenneListe1 = calculerMoyenne(sousListes[0], taillesSousListes[0]);
	//printf("%f\n", moyenneListe1);
    double moyenneListe2 = calculerMoyenne(sousListes[1], taillesSousListes[1]);
	//printf("%f\n", moyenneListe2);
	if (moyenneListe1 > moyenneListe2)
	{
		double temp = moyenneListe1;
		moyenneListe1 = moyenneListe2;
		moyenneListe2 = temp;
	}
	int verife = 0;
     if (fabs(moyenneListe1 - 0) < fabs(moyenneListe1 - 90)) {
        // La valeur est plus proche de 0
        moyenneListe1 = moyenneListe1;
    } else {
        // La valeur est plus proche de 90
        moyenneListe1 = 90 - moyenneListe1;
		verife = 1;
    }

    if (fabs(moyenneListe2 - 90) < fabs(moyenneListe2 - 180)) {
        // La valeur est plus proche de 90
        moyenneListe2 = moyenneListe2 - 90;
    } else {
        // La valeur est plus proche de 180
        moyenneListe2 = 180 - moyenneListe2;
    }
	//printf("%f\n", moyenneListe1);
	//printf("%f\n", moyenneListe2);
    double angle = (moyenneListe1 + moyenneListe2) / 2;
	if (verife == 1)
	{
		angle *= - 1;
	}
	int new_angle = (int)(angle);
   	//printf("%d\n", new_angle);
   	// Add border to keep all informations
	    SDL_Surface* image_with_border = enlargeImageWithRedBackground(image,350);
	
    	// Rotated image
    	SDL_Surface* rotatedImage = rotateImage(image_with_border, new_angle);
		
		IMG_SaveJPG(rotatedImage,"rotation.jpg",100);
    	
    	// Free memory
    	SDL_FreeSurface(image);
		//SDL_FreeSurface(whiteSurface);
		SDL_FreeSurface(image_with_border);
    	SDL_FreeSurface(rotatedImage);

    	// Close SDL
    	SDL_Quit();

    	return 0;
}
