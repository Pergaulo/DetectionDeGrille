CC = gcc
CFLAGS = -std=c99 -Wall -pedantic `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

all: rotation

rotation: auto_rotation.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f rotation
