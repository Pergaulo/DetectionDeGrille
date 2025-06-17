#include <stdio.h>
#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define PI 3.14159265358979323846

typedef struct tuple
{
	int a;
	int b;
}tuple;

typedef struct square{
    int x1;
    int y1;
    int x2;
    int y2;
    int x3;
    int y3;
    int x4;
    int y4;
    double size;
} square;


Uint8* pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
	//recupere les pixels
	int bpp = surf->format->BytesPerPixel;
	return (Uint8*)surf->pixels + y * surf->pitch + x * bpp;
}

void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	//remettre un pixel
	Uint8 *p = pixel_ref(surface, x, y);

	switch(surface->format->BytesPerPixel)
	{
		case 1:
			*p = pixel;
			break;

		case 2:
			*(Uint16 *)p = pixel;
			break;

		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			{
				p[0] = (pixel >> 16) & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = pixel & 0xff;
			}
			else
			{
				p[0] = pixel & 0xff;
				p[1] = (pixel >> 8) & 0xff;
				p[2] = (pixel >> 16) & 0xff;
			}
			break;

		case 4:
			*(Uint32 *)p = pixel;
			break;
	}
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
    
    

void drawLine(int x1, int y1, int x2, int y2, SDL_Surface *surface) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx, sy;

    if (x1 < x2) {
        sx = 1;
    } else {
        sx = -1;
    }

    if (y1 < y2) {
        sy = 1;
    } else {
        sy = -1;
    }

    int err = dx - dy;

    while (x1 != x2 || y1 != y2) {
        // Dessiner un pixel à (x1, y1) sur la surface
        if (x1 >= 0 && x1 < surface->w && y1 >= 0 && y1 < surface->h) {
            Uint32 pixel = SDL_MapRGB(surface->format,255,0, 0);
            int bpp = surface->format->BytesPerPixel;
            Uint8 *p = (Uint8 *)surface->pixels + y1 * surface->pitch + x1 * bpp;

            switch (bpp) {
                case 1:
                    *p = pixel;
                    break;
                case 2:
                    *(Uint16 *)p = pixel;
                    break;
                case 3:
                    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                        p[0] = (pixel >> 16) & 0xff;
                        p[1] = (pixel >> 8) & 0xff;
                        p[2] = pixel & 0xff;
                    } else {
                        p[0] = pixel & 0xff;
                        p[1] = (pixel >> 8) & 0xff;
                        p[2] = (pixel >> 16) & 0xff;
                    }
                    break;
                case 4:
                    *(Uint32 *)p = pixel;
                    break;
            }
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

SDL_Surface* load_image(char *path)
{
	SDL_Surface *img;

	// Load an image using SDL_image with format detection.
	// If it fails, die with an error message.
	img = IMG_Load(path);
	if (!img)
		errx(3, "can't load %s: %s", path, IMG_GetError());

	return img;
}


SDL_Surface * hough(SDL_Surface *surface, int x, int y)
{
	//On commence par définir l'accumulateur. rho et theta sont les résolutions, respectivement en pixel et en degré.
	int Ny = y;
	int Nx = x;
	double rho = 1.0;
	double theta = 1.0;
	int Ntheta = (int)(180.0/theta);
	int Nrho = (int)(sqrt(Nx * Nx + Ny * Ny) / rho);
	double dtheta = PI/Ntheta;
	//printf("laaaaaaaaa : %f\n",dtheta);
	double drho = (sqrt(Nx * Nx + Ny * Ny) / Nrho);
	//printf("\nddd : %f,%f\n",dtheta,drho);
	int accum[Ntheta][Nrho];
	// Initialiser le tableau avec des zéros
	for (int i = 0; i < Ntheta; i++) {
		for (int j = 0; j < Nrho; j++) {
		     accum[i][j] = 0;
		     }
	}
	

	//Pour chaque pixel noir de l'image, on incrémente les points de l'accumulateur obtenus en échantillonnant la sinusoïde : 
	for (int j = 0; j < Ny; j++) 
	{
		    for (int i = 0; i < Nx; i++) 
			{
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
	int dist_theta = 1;
	int dist_rho = 26;
	int accum_seuil[Ntheta][Nrho];
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++){
		        accum_seuil[i_theta][i_rho] = accum[i_theta][i_rho];
		}
	}
	
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
		    if (accum[i_theta][i_rho] < seuil)
		        accum_seuil[i_theta][i_rho] = 0;
		}
	}
    //close line
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
			
		    if (accum_seuil[i_theta][i_rho] != 0)
		        {

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

	//deleate diagonal lines 
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
		    if ((i_theta  > 1 && i_theta < 89) || (i_theta > 91))
		    {
				accum_seuil[i_theta][i_rho] = 0; 
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
	//check close line with same seuil 
	/*
	for (int z = 0; z < ind_1;z++)
	{
		printf("t %d, ",l[z][0]);
		fflush(stdout);
		printf("r %d, ",l[z][1]);
		fflush(stdout);
		printf("s %d\n",l[z][2]);
		fflush(stdout);
	}*/
	
	
	int seuil_temp = 0;
	for (int i = 0; i < ind_1; i++)
	{
		int j = i;
		if (l[j][2] != seuil_temp)
		{
			seuil_temp = l[j][2];
		}
		else
		{
			int c = 1;
			int dist = 10;
			while(j < ind_1 - 1 && l[j][2] == l[j + 1][2] && l[j][1] + dist > l[j + 1][1] && l[j][1] - dist < l[j + 1][1])
			{
				
				c ++;
				j++;
			}
			if (c != 1) 
			{
				int mid = c/2 + i - 1;
				for (int same = i - 1; same < c + i; same ++)
				{
					//printf("1 : %d, 2 : %d\n",l[same][2], l[mid][2]);
					if (same != mid)
					{
						l[same][2] = 0;
					}
				}
				i += c - 1;
			}
		}
	}
	for (int i_l = 0; i_l < ind_1; i_l++)
	{
		accum_seuil[l[i_l][0]][l[i_l][1]] = l[i_l][2];
	}  

	//printf("\n\n");
	/*for (int z = 0; z < ind_1;z++)
	{
		printf("t %d, ",l[z][0]);
		fflush(stdout);
		printf("r %d, ",l[z][1]);
		fflush(stdout);
		printf("s %d\n",l[z][2]);
		fflush(stdout);
	}*/

	

	//On constate qu'il y a une forte accumulation autour de certains points, alors que d'autres sont à peine visibles. Pour bien voir les droites obtenues, nous allons placer les couples (ρ,θ) dans une liste : 
	
	size_t count = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++){
		for (int i_rho = 0; i_rho < Nrho; i_rho++){
		    if (accum_seuil[i_theta][i_rho] != 0)
		    	{
		    		count++;
		    	}
		}
	}
	int* list_theta = malloc((count + 1)* sizeof(int));
	list_theta[0] = count;
	double lignes[count][2];
	size_t i = 0;
	for (int i_theta = 0; i_theta < Ntheta; i_theta++)
	{
		for (int i_rho = 0; i_rho < Nrho; i_rho++)
		{
		    if (accum_seuil[i_theta][i_rho] != 0)
		    	{
		    		lignes[i][0] = i_rho * drho;
		    		lignes[i][1] = i_theta * dtheta;
					list_theta[i + 1] = i_theta;
		    		//printf("ir : %d dr : %f res : %f\n",i_rho, drho,i_rho * drho);
		    		//printf("it : %d dt : %f tes : %f\n",i_theta, dtheta,i_theta * dtheta);
		    		i++;
		    	}
		}
	}
	
	fflush(stdout);
	//printf(r.a);
	// Enfin on trace ces droites :
	for (int j = 0; j < count; j++)
	{
		double rho = lignes[j][0];
		double theta = lignes[j][1];
		//printf("t%d  r%d\n",theta,rho);
		double a = cos(theta);
		double b = sin(theta);
		double x0 = a*rho;
		double y0 = b*rho;
		//printf("a%d  b%d  x0%d  y0%d\n",a,b,x0,y0);
		int x1 = (int)(x0 + x * 3*(-b));
		int y1 = (int)(y0 + x * 3*(a));
		int x2 = (int)(x0 - x * 3*(-b));
		int y2 = (int)(y0 - x * 3*(a));
		drawLine(x1, y - y1, x2, y - y2, surface);
  	 }  
	return surface;
}

struct tuple* intersection(SDL_Surface* surface, int x, int y)
{
	int size_l = 0;
	for (int i = 0; i < x;i++)
	{
		for (int j = 0; j < y; j++)
		{
			Uint32 pixel = get_pixel(surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			if (r == 255 && g == 0 && b == 0)
			{
				int adj = 0;
				for (int a = -1; a < 2; a++)
				{
					for (int b = -1; b < 2; b++)
					{
						if ( i + a >= 0 && b + j >= 0 && i + a < x && j + b < y)
						{
							Uint32 pixel = get_pixel(surface, i + a, j + b);
            				Uint8 r, g, b;
							SDL_GetRGB(pixel, surface->format, &r, &g, &b);
							if (r == 255 && g == 0 && b == 0)	
								adj ++;
						}
					}
				}
				if (adj == 5)
				{
					size_l++;
				}
			}
		}
	}

    struct tuple* inter = malloc(size_l * sizeof(struct tuple));
    if (inter == NULL) {
        fprintf(stderr, "Erreur : échec de l'allocation de mémoire pour inter.\n");
        exit(EXIT_FAILURE);
    }
	int n = 0;
	for (int i = 0; i < x;i++)
	{
		for (int j = 0; j < y; j++)
		{
			Uint32 pixel = get_pixel(surface, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, surface->format, &r, &g, &b);
			if (r == 255 && g == 0 && b == 0)
			{
				int adj = 0;
				for (int a = -1; a < 2; a++)
				{
					for (int b = -1; b < 2; b++)
					{
						if ( i + a >= 0 && b + j >= 0 && i + a < x && j + b < y)
						{
							Uint32 pixel = get_pixel(surface, i + a, j + b);
            				Uint8 r, g, b;
							SDL_GetRGB(pixel, surface->format, &r, &g, &b);
							if (r == 255 && g == 0 && b == 0)	
								adj ++;
						}
					}
				}
				if (adj == 5)
				{
					struct tuple t;
					t.a = i;
					t.b = j;
					inter[n] = t; 
					n++;
				
					for (int a = -3; a < 4; a++)
					{
						for (int b = -3; b < 4; b++)
						{
							if ( i + a >= 0 && b + j >= 0 && i + a < x && j + b < y)
							{
								Uint32 pixel = SDL_MapRGB(surface->format, 0, 255, 0);
								put_pixel(surface,i + a,j + b,pixel);
							}
						}
					}
				}
				
			}
		}
	}
	//printf("\n\n");
	struct tuple* new_insert = malloc((n + 1) * sizeof(struct tuple));
    if (new_insert == NULL) {
        fprintf(stderr, "Erreur : échec de l'allocation de mémoire pour new_insert.\n");
        exit(EXIT_FAILURE);
    }
	new_insert[0].a = n;
	new_insert[0].b = n;
	for (int i = 0; i < n;i++)
	{
		new_insert[i + 1].a = inter[i].a;
		new_insert[i + 1].b = inter[i].b;
	}
	//printf("\n\n");
	//printf("%d,%d\n",n,size_l);
	return new_insert;
}


square trouver_plus_grand_carre(SDL_Surface* surface, struct tuple* inter, int size, int x, int y) 
{
    square maxi_square = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < size; i++) 
	{
        for (int j = 0; j < size; j++)
		{
			if (j != i)
			{
				for (int k = 0; k < size;  k++)
				{
					if (k != j && k != i)
					{
				 		for (int l=  0; l < size; l++) 
						{
							if (l != i && l != k && l != j)
							{
								int x1 = inter[i].a;
								int y1 = inter[i].b;
								int x2 = inter[j].a;
								int y2 = inter[j].b;
								int x3 = inter[k].a;
								int y3 = inter[k].b;
								int x4 = inter[l].a;
								int y4 = inter[l].b;
								int dist = 12;
								double cote = 0;
								int check = 0;
								if (y1 + dist > y2 && y1 - dist < y2 )
								{
									cote += abs(x2 - x1);
									//printf("1");
									if (y3 + dist > y4 && y3 - dist < y4 )
									{
										cote += abs(x4 - x3);
										//printf("2");
										if (x1 + dist > x3  && x1 - dist < x3 )
										{
											cote += abs(y3 - y1);
											//printf("3");
											if (x2 + dist > x4 && x2 - dist < y4 )
											{
												cote += abs(y4 - y2);
												check ++;
												//printf("%f\n",cote);
											}
										}
									}
								}
								cote /= 4;
								if (check == 1 && cote > maxi_square.size)
								{
									//printf("ok : %f\n", cote);
									

									if (abs(x2 - x1) + dist > cote && abs(x2 - x1) - dist < cote)
									{
										//printf("1");
										if (abs(x4- x3) + dist > cote && abs(x4 - x3) - dist < cote)
										{
											//printf("2");
											if (abs(y3 - y1) + dist > cote && abs(y3 - y1) - dist < cote)
											{
												//printf("3");
												if (abs(y4 - y2) + dist > cote && abs(y4 - y2) - dist < cote)
												{
													check ++;
													//printf("4");
												    //printf("(%d, %d)\n",x1, y1);
													//printf("(%d, %d)\n", x2, y2);
													//printf("(%d, %d)\n", x3, y3);
													//printf("(%d, %d)\n", x4, y4);
													//printf("%f\n",cote);
												}
											}
										}
									}

									if (check == 2)
									{
										
										maxi_square.x1 = x1;
										maxi_square.y1 = y1;
										maxi_square.x2 = x2;
										maxi_square.y2 = y2;
										maxi_square.x3 = x3;
										maxi_square.y3 = y3;
										maxi_square.x4 = x4;
										maxi_square.y4 = y4;
										maxi_square.size = cote;
										
									}	
								}
							}	
						}
			  		}
			  	}
			}
        }
    }
	for (int a = -10; a < 11; a++)
	{
		for (int b = -10; b < 11; b++)
		{
			if ( maxi_square.x1  + a >= 0 && b + maxi_square.y1  >= 0 && maxi_square.x1 + a < x && maxi_square.y1 + b < y)
			{
				Uint32 pixel = SDL_MapRGB(surface->format, 0, 255, 255);
				put_pixel(surface,maxi_square.x1  + a,maxi_square.y1  + b,pixel);
			}
		}
	}
	for (int a = -10; a < 11; a++)
	{
		for (int b = -10; b < 11; b++)
		{
			if ( maxi_square.x2  + a >= 0 && b + maxi_square.y2  >= 0 && maxi_square.x2 + a < x && maxi_square.y2 + b < y)
			{
				Uint32 pixel = SDL_MapRGB(surface->format, 0, 255, 255);
				put_pixel(surface,maxi_square.x2  + a,maxi_square.y2  + b,pixel);
			}
		}
	}
	for (int a = -10; a < 11; a++)
	{
		for (int b = -10; b < 11; b++)
		{
			if ( maxi_square.x3  + a >= 0 && b + maxi_square.y3  >= 0 && maxi_square.x3 + a < x && maxi_square.y3 + b < y)
			{
				Uint32 pixel = SDL_MapRGB(surface->format, 0, 255, 255);
				put_pixel(surface,maxi_square.x3  + a,maxi_square.y3  + b,pixel);
			}
		}
	}
	for (int a = -10; a < 11; a++)
	{
		for (int b = -10; b < 11; b++)
		{
			if ( maxi_square.x4  + a >= 0 && b + maxi_square.y4  >= 0 && maxi_square.x4 + a < x && maxi_square.y4 + b < y)
			{
				Uint32 pixel = SDL_MapRGB(surface->format, 0, 255, 255);
				put_pixel(surface,maxi_square.x4  + a,maxi_square.y4  + b,pixel);
			}
		}
	}
	//printf("%d,%d,%d,%d",maxi_square.x2 - maxi_square.x1, maxi_square.x4- maxi_square.x3, maxi_square.y3 - maxi_square.y1, maxi_square.y4 - maxi_square.y2);
    return maxi_square;
}

struct square select_coord(struct square m)
{
	struct square res;
	int num1 = m.x1 + m.y1;
	int num2 = m.x2 + m.y2;
	int num3 = m.x3 + m.y3;
	int num4 = m.x4 + m.y4;
	if (num1 < num2 && num1 < num3 && num1 < num4)
	{
		res.x1 = m.x1;
		res.y1 = m.y1;
	}
	else if (num2 < num1 && num2 < num3 && num2 < num4)
	{
		res.x1 = m.x2;
		res.y1 = m.y2;
	}
	else if (num3 < num2 && num3 < num1 && num3 < num4)
	{
		res.x1 = m.x3;
		res.y1 = m.y3;
	}
	else 
	{
		res.x1 = m.x4;
		res.y1 = m.y4;
	}
	return res;
}
int main(int argc, char** argv) {
	//Check argument
	//printf("cccc");
	fflush(stdout);
	if (argc != 2)
        errx(EXIT_FAILURE, "error : need 2 arguments");
        
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur d'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    // Load image
    SDL_Surface* image = load_image(argv[1]);
    if (image == NULL) {
        fprintf(stderr, "Erreur de chargement de l'image : %s\n", IMG_GetError());
        return 1;
    }
   	
	
    // ROTATION
	//find theta
	
	//apply the rotation

	//DETECT LINES
	SDL_Surface* temp =  load_image(argv[1]);
	SDL_Surface* line_surface = hough(image, image->w, image->h);
	
	//DETECT INTERSECTIONS
    struct tuple* insert = intersection(image, image->w, image->h);


	struct tuple *inter = malloc(insert[0].a * sizeof(struct tuple));

	if (insert == NULL) 
	{
    fprintf(stderr, "Erreur : échec de l'allocation de mémoire pour insert.\n");
    exit(EXIT_FAILURE);
	}



	for (int i = 0; i < insert[0].a;i++)
	{
		inter[i].a = insert[i + 1].a;
		inter[i].b = insert[i + 1].b;
	}
/*
	for (int j = 0; j < insert[0].a;j++)
	{
		printf("%d,%d\n",inter[j].a,inter[j].b);
		fflush(stdout);
	}*/
	
	//DETECT SQUARE
	square plus_grand_carre = trouver_plus_grand_carre(line_surface, inter, insert[0].a,line_surface->w, line_surface->h);
    
    /*printf("Coins du plus grand carré :\n");
    printf("(%d, %d)\n", plus_grand_carre.x1, plus_grand_carre.y1);
    printf("(%d, %d)\n", plus_grand_carre.x2, plus_grand_carre.y2);
    printf("(%d, %d)\n", plus_grand_carre.x3, plus_grand_carre.y3);
    printf("(%d, %d)\n", plus_grand_carre.x4, plus_grand_carre.y4);
    printf("Taille du plus grand carré : %f\n", plus_grand_carre.size);*/

	struct square res = select_coord(plus_grand_carre);
	//CROP IMAGE

	int crop_size = (int)plus_grand_carre.size;
	int crop_x = res.x1;
	int crop_y = res.y1;

	SDL_Surface* cropped_img = SDL_CreateRGBSurface(0, crop_size + 10, crop_size + 10, 32, 0, 0, 0, 0);
    for (int y = 0; y < crop_size + 10; y++) 
	{
        for (int x = 0; x < crop_size + 10; x++) 
		{
			if (x >= 0 && x < image->w && y >= 0 && y < image->h)
			{
				Uint32 pixel = get_pixel(temp, x + crop_x - 5, y + crop_y - 5);
				put_pixel(cropped_img,x,y,pixel);
			}
        }
    }

    // Sauvegarder l'image rognée
    IMG_SaveJPG(cropped_img,"cropped.jpg",100);

    // Libérer la mémoire
    free(cropped_img);

	//Save
	IMG_SaveJPG(line_surface,"line.jpg",100);

   	//Free
    SDL_FreeSurface(image);
	SDL_FreeSurface(line_surface);

    // Close SDL
    SDL_Quit();

    return 0;
}
            
            

