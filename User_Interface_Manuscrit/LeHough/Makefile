CC = gcc
LDLIBS =  -lm -ldl -lSDL2 -lSDL2_image

# Spécifiez le chemin d'accès aux fichiers d'en-tête de SDL et de SDL_image
all: hough

hough: hough2.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

clean:
	rm -f hough
	rm line.jpg
