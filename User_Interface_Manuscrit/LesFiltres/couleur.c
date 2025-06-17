#include <err.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


SDL_Surface* load_image(const char* path)
{
    SDL_Surface* surface_temp = IMG_Load(path);
    SDL_Surface* surface = SDL_ConvertSurfaceFormat(surface_temp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(surface_temp);
    return surface;
}

void draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
	SDL_RenderCopy(renderer,texture,NULL,NULL);
	SDL_RenderPresent(renderer);
}


Uint32 pixel_to_blackandwhite(Uint32 pixel_color, SDL_PixelFormat* format,float seuil)
{
  	Uint8 r, g, b;
    Uint32 color;
	  SDL_GetRGB(pixel_color, format, &r, &g, &b);
    if ((float)(r + b + g)/3>= seuil)
    {
        color = SDL_MapRGB(format,255,255,255);
    }
    else
    {
        color = SDL_MapRGB(format,0,0,0);
    }    
	return color;
}


Uint32 pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
	Uint8 r, g, b;
	SDL_GetRGB(pixel_color, format, &r, &g, &b);
	//Uint8  average = 0.3*r + 0.59*g + 0.11*b;
  	Uint8  average = 0.299*r + 0.587*g + 0.114*b;
	Uint32 color = SDL_MapRGB(format, average, average, average);
	return color;
}


void surface_to_grayscale(SDL_Surface* surface)
{
	Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;
	SDL_LockSurface(surface);
	for(int i = 0;i<len;i++)
	{
		pixels[i] = pixel_to_grayscale(pixels[i],format);
	}
	SDL_UnlockSurface(surface);

}

void init_hist(SDL_Surface* surface,int* hist,int height2, int width2,int i2 , int j2)
{

  for (size_t i = 0; i <= 255; i++)
  {
    hist[i]=0;
  }

  Uint32* pixels = surface->pixels;
	int width = surface->w ;
  SDL_PixelFormat* format = surface->format;
  for (int i = i2; i <height2 ; i++)
  {
    for (int j = j2; j < width2; j++)
    {
      Uint8 r, g, b;
  	  SDL_GetRGB(pixels[i*width+j], format, &r, &g, &b);
      hist[r] += 1;
    }
  }
}

void surface_to_blackandwhite(SDL_Surface* surface,float seuil)
{
  Uint32* pixels = surface->pixels;
	int len = surface->w * surface->h;
	SDL_PixelFormat* format = surface->format;
	SDL_LockSurface(surface);
	for(int i = 0;i<len;i++)
	{
		pixels[i] = pixel_to_blackandwhite(pixels[i],format,seuil);
	}
	SDL_UnlockSurface(surface);
}


void event_loop(SDL_Renderer* renderer, SDL_Texture* grayscale)
{
    SDL_Event event;

    while (1)
    {
        SDL_WaitEvent(&event);

        switch (event.type)
        {
		case SDL_QUIT:
			return;
		case SDL_WINDOWEVENT:
			if(event.window.event == SDL_WINDOWEVENT_RESIZED)
				draw(renderer, grayscale);
			break;
		
        }
    }
}


float moy_hist(int* hist)
{
  unsigned long sum = 0;
  unsigned long count = 0;
  for (size_t i = 0; i <= 255; i++)
  { 
    sum+=i*hist[i];
    count += hist[i];
  }
  return sum / count;
}

float min_hist(int* hist)
{
  unsigned long min = 255;
  for (int i = 255; i >= 0; i--)
  { 
    if (hist[i]>0)
    {
      min = i;
    }
  }
  return min;
}

float max_hist(int* hist)
{
  unsigned long max = 0;
  for (int i = 0; i <= 255; i++)
  { 
    if (hist[i]>0)
    {
      max = i;
    }
  }
  return max;
}

double pow_me(double val1,double val2)
{
  double res =val1;
  for (size_t i = 1; i < val2; i++)
  {
    res*=val1;
  }
  return val1;
}

int main(int argc, char** argv)
{
    // Checks the number of arguments.
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage: image-file");

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* surface = load_image(argv[1]);


    surface_to_grayscale(surface);
    //IMG_SaveJPG(surface,"test_grayscale.jpg",100);
    
   
    int width = surface->w;
    int height = surface->h;
    int hist[256];
    init_hist(surface,hist,height,width,0,0);
    float min = min_hist(hist);
    //float max = max_hist(hist);
    float moy = moy_hist(hist);
    float seuil = moy*0.75+min/2;
    if (seuil>178)
    {
      seuil = 178;
    }
    
    surface_to_blackandwhite(surface,seuil);
    init_hist(surface,hist,height,width,0,0);

    IMG_SaveJPG(surface,"test_blackandwhite.jpg",100);

    

    SDL_FreeSurface(surface);

    SDL_Quit();

    // - Initialize the SDL.
    // - Create a window.
    // - Create a renderer.
    // - Create a surface from the colored image.
    // - Resize the window according to the size of the image.
    // - Create a texture from the colored surface.
    // - Convert the surface into grayscale.
    // - Create a new texture from the grayscale surface.
    // - Free the surface.
    // - Dispatch the events.
    // - Destroy the objects.
    return EXIT_SUCCESS;
}
