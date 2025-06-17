#include <err.h>
#include <SDL.h>
#include <SDL2/SDL_image.h>

typedef struct tuple
{
	int nb;
	int lon;
}tuple;


SDL_Surface* load_image(char *path)
{
	SDL_Surface *image;

	// Load an image
	image = IMG_Load(path);
	if (!image)
		errx(3, "can't load %s: %s", path, IMG_GetError());
	return image;
}

Uint8* pixel_ref(SDL_Surface *surface, unsigned x, unsigned y)
{
	int bpp = surface->format->BytesPerPixel;
	return (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
}

Uint32 get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
	Uint8 *p = pixel_ref(surface, x, y);
	
	//the surface can has a different type
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

void put_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	Uint8 *p = pixel_ref(surface, x, y);
	//the surface can has a different type
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

SDL_Surface* print_image(SDL_Surface *image_line)
{
	IMG_SaveJPG(image_line,"segment.jpg",100);
}

void setlines(SDL_Surface *image, int *x, int *y)
{
	Uint32 pixel = SDL_MapRGB(image->format, 255, 0, 0);
	for(int i = 0; i < 10; i++)
	{
		int xx = y[i];
		for(int j = 0; j < image->w;j++)
		{
			put_pixel(image,j,xx,pixel);
		}
	}
	for(int i = 0; i < 10; i++)
	{
		int xx = x[i];
		for(int j = 0; j < image->h;j++)
		{
			put_pixel(image,xx,j,pixel);
		}
	}
}

void pixel_by_lines(SDL_Surface *image, int width, int height, int* lines_horizontal, int* lines_vertical)
{
    int n_h = 0;
    int n_v = 0;

    // horizontal lines
    for(int y = 0; y < height ; y++) 
    {
        for(int x = 0; x < width ; x++)
        {
            int no_cont = 0;
            Uint32 pixel = get_pixel(image,x,y);
            Uint8 r,g,b;
            SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
            if((r == 0)&&(g == 0)&&(b == 0))
            {
                   n_h++;
            }
        }
        lines_horizontal[y]  = n_h;
        n_h = 0;
    }

    //vertical lines
    for(int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            Uint32 pixel = get_pixel(image,x,y);
            Uint8 r,g,b;
            SDL_GetRGB(pixel, image->format, &(r), &(g), &(b));
            if((r == 0)&&(g == 0)&&(b == 0))
            {
                n_v++;
            }
        }
        lines_vertical[x]  = n_v;
        n_v = 0;
    }
}

void getlines(SDL_Surface *image, int* x, int* y)
{
    int width = image->w;
    int height = image->h;
    int* lines_horizontal = malloc(height * sizeof(int));
    int* lines_vertical = malloc(width * sizeof(int));
    pixel_by_lines(image, width, height, lines_horizontal, lines_vertical);

   
    for(int i = 0; i < 12;i++)
    {
        int ind = 0;
        int max = 0;
        //int max_lon = 300;
        for(int x = 0; x < height; x++)
        {
            if(lines_horizontal[x]  > max)
            {
                max = lines_horizontal[x];
                //max_lon = lines_horizontal[x].lon;
                ind = x;
            }
        }
        for(int j = ind - 15; j < ind + 15; j++)
        {
            //deleate close line
            if(j > -1 && j < height)
                lines_horizontal[j] = 0;
        }
        y[i] = ind;
    }


    for(int i = 0; i < 12;i++)
    {
        int ind = 0;
        int max = 0;
        int max_lon = 300;
        for(int y = 0; y < width; y++)
        {
            if(lines_vertical[y] > max)
            {
                max = lines_vertical[y] ;
                //max_lon = lines_vertical[y].lon;
                ind = y;
            }
        }
        for(int j = ind - 15; j < ind + 15; j++)
        {
            //deleate close line
            if(j > - 1 && j < width)
                lines_vertical[j]  = 0;
        }
        x[i] = ind;
    }

    free(lines_horizontal);
    free(lines_vertical);
}

int dif(const void *a,const void *b) 
{
	int *x = (int *) a;
	int *y = (int *) b;
	return *x - *y;
}

void transformBorderPixels(SDL_Surface *image, int borderWidth) {
    if (image == NULL || borderWidth <= 0) {
        return;
    }
    int width = image->w;
    int height = image->h;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x < borderWidth || x >= width - borderWidth || y < borderWidth || y >= height - borderWidth) {
                Uint32 pixel = get_pixel(image, x, y);
                Uint8 r, g, b;
                SDL_GetRGB(pixel, image->format, &r, &g, &b);

                if ((r == 0 && g == 0 && b == 0) || (r == 255 && g == 0 && b == 0)) {
                    put_pixel(image, x, y, SDL_MapRGB(image->format, 255, 255, 255));
                }
            }
        }
    }
}

int abs_diff(int a, int b)
{
    return abs(a - b);
}

void segmentation(SDL_Surface *image) {
	int borderWidth = 15;
    int *x = malloc(12 * sizeof(int));
    int *y = malloc(12 * sizeof(int));

    getlines(image, x, y);

    qsort(x, 12, sizeof(int), dif);
    qsort(y, 12, sizeof(int), dif);

    //CHECK LINES ON X
    double moy = 0;
    int div = 0;
    for (int i = 0; i < 11; i++)
    {
        int res = abs_diff(x[i], x[i+1]); 
        if (res > 55)
            {   
                moy += res;
                div ++;
            }

    }
    if (div > 0)
        moy /= div;
    double dist = 15;
    int *x_step = malloc(11 * sizeof(int));
    int n = 0;
    for (int i = 0; i < 11; i++)
    {
        //printf("%d, %f \n",abs_diff(x[i], x[i+1]),moy);
        if (abs_diff(x[i], x[i+1]) + dist > moy && abs_diff(x[i], x[i+1]) - dist < moy)
        {
            x_step[i] = 1;
        }
        else
        {
            x_step[i] = 0;
        }
    }

   /* for(int i = 0; i < 11; i++)
    {
        printf("%d\n",x_step[i]);
    }*/

    int* x_valide = malloc(12 * sizeof(int));
    x_valide[0] = 2;
    for(int i = 0; i < 11; i++)
    {
        if (x_step[i] == 0 && x_step[i + 1] == 0)
        {
            //printf("x : deleate line %d\n",i + 1);
            n++;
            x_valide[i+1] = -1;
        }
        //printf("i : %d\n",i);
	else
		x_valide[i+1] = 0;
    }
    x_valide[11] = 2;
    if (n < 2)
    {
        moy = 0;
        int c = 0;
        for (int i = 0; i < 11; i++)
        {
            if (x_valide[i] != - 1 && x_valide[i + 1] != -1)
            {
                int res = abs_diff(x[i], x[i+1]); 
                //printf("dif : %d",res);
                moy += res;
                c++;
            }
        }
        if (c != 0)
            moy /= c;
        if (n == 1)
        {
            double dist1 = abs_diff(x[11],x[10]);
            double dist2 = abs_diff(x[0],x[1]);

            if (fabs(dist1 - moy) > fabs(dist2 - moy))
            {
                //printf("x : deleate line 10\n");
                x_valide[11] = -1;
            }
            else
            {
                //printf("x : dealeate line 0\n");
                x_valide[0] = -1;
            }
        }

        else if (n == 0)
        {
            //printf("x : deleate line 11\n");
            x_valide[11] = -1;
            //printf("x : deleate line 0\n");
            x_valide[0] = -1;
        }
    }

   
    
    int *x_step_deleate = malloc(10 * sizeof(int));
    int moins = 0;
    for (int i = 0; i < 12; i++)
    {
        if (x_valide[i] != -1)
        {
            x_step_deleate[i - moins] = x[i];
        }
        else
        {
            moins++;
        }
    }

     for (int i = 0; i < 12;i++)
    {
        //printf("x : %d\n", x[i]);
    }
      for (int i = 0; i < 12;i++)
    {
        //printf("x_s : %d\n", x_step[i]);
    }


    /*for (int i = 0; i < 10;i++)
    {
        printf("x_d : %d\n", x_step_deleate[i]);
    }*/


//CHECK LINES ON Y

    moy = 0;
    div = 0;
    for (int i = 0; i < 11; i++)
    {
        int res = abs_diff(y[i], y[i+1]); 
        if(res > 60)
        {
            div ++;
            moy += res;
        }
   }
    if(div > 0)
        moy /= div;
    int *y_step = malloc(11 * sizeof(int));
    n = 0;
    for (int i = 0; i < 11; i++)
    {
        if (abs_diff(y[i], y[i+1]) + dist > moy && abs_diff(y[i], y[i+1]) - dist < moy)
        {
            y_step[i] = 1;
        }
        else
        {
            y_step[i] = 0;
        }
    }

   /* for(int i = 0; i < 11; i++)
    {
        printf("%d\n",x_step[i]);
    }*/
    int* y_valide = malloc(12 * sizeof(int));
    y_valide[0] = 2;
    for(int i = 0; i < 11; i++)
    {
        if (y_step[i] == 0 && y_step[i + 1] == 0)
        {
            //printf("y : deleate line : %d\n",i + 1);
            n++;
            y_valide[i+1] = -1;
        }
	else
		y_valide[i+1] = 1;
        //printf("i : %d\n",i);
        
    }
    y_valide[11] = 2;
    if (n < 2)
    {
        moy = 0;
        int c = 0;
        for (int i = 0; i < 11; i++)
        {
            if (y_valide[i] != - 1)
            {
                int res = abs_diff(y[i], y[i+1]); 
                moy += res;
                c++;
            }
        }
        if (c != 0)
            moy /= (c);
        if (n == 1)
        {
            double dist1 = abs_diff(y[11],y[10]);
            double dist2 = abs_diff(y[0],y[1]);
            if (fabs(dist1 - moy) > fabs(dist2 - moy))
            {
                //printf("y : deleate line 10\n");
                y_valide[11] = -1;
            }
            else
            {
                //printf("y : dealeate line 0\n");
                y_valide[0] = -1;
            }
        }

        else if (n == 0)
        {
            //printf("y : deleate line 11\n");
            y_valide[11] = -1;
            //printf("y : deleate line 0\n");
            y_valide[0] = -1;
        }
    }
    
    int *y_step_deleate = malloc(10 * sizeof(int));
    moins = 0;
    for (int i = 0; i < 12; i++)
    {
        if (y_valide[i] != -1)
        {
            y_step_deleate[i - moins] = y[i];
        }
        else
        {
            moins++;
        }
    }

     /*for (int i = 0; i < 12;i++)
    {
        printf("y : %d\n", y[i]);
    }*/
      for (int i = 0; i < 12;i++)
    {
        //printf("y_s : %d\n", y_step[i]);
    }


    /*for (int i = 0; i < 10;i++)
    {
        printf("y_d : %d\n", y_step_deleate[i]);
    }*/
    setlines(image, x_step_deleate, y_step_deleate);
    
    //printf("pergoat\n");
    print_image(image);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            SDL_Rect rectangle;
            rectangle.x = x_step_deleate[j];
            rectangle.y = y_step_deleate[i];
            rectangle.w = x_step_deleate[j + 1] - x_step_deleate[j];
            rectangle.h = y_step_deleate[i + 1] - y_step_deleate[i];
            
            SDL_Surface *cropped = SDL_CreateRGBSurface(SDL_SWSURFACE, rectangle.w, rectangle.h, 32, 0, 0, 0, 0);
            SDL_BlitSurface(image, &rectangle, cropped, NULL);

            //cur black border
            transformBorderPixels(cropped, borderWidth);

            char buffer[100];
            snprintf(buffer, sizeof(buffer), "./%d%d.bmp", i,j);

            if (SDL_SaveBMP(cropped, buffer) != 0)
            {
                printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
            }
            SDL_FreeSurface(cropped);
            //printf("%d,%d\n",j,i);
            fflush(stdout);
            }
    }
    system("mv *.bmp image_test_nn/");

    free(y_valide);
    free(x_valide);
    free(x_step);
    free(y_step);
    free(x_step_deleate);
    free(y_step_deleate);
    free(x);
    free(y);
    SDL_FreeSurface(image);
}


int main(int argc, char **argv)
{
	if (argc != 2){
		errx(1,"the segmentation function takes 2 parameters");
		return EXIT_FAILURE;
	}
	if(SDL_Init(SDL_INIT_VIDEO) == -1)
		errx(1,"Could not initialize SDL: %s.\n", SDL_GetError());
	
	SDL_Surface* image_line = load_image(argv[1]);
	segmentation(image_line);
	return 0;
}
