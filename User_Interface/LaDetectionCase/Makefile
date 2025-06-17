CC = gcc
CFLAGS = -std=c99 -Wall -pedantic `sdl2-config --cflags` -g
LDFLAGS = `sdl2-config --libs` -lSDL2_image -lm

all: output segmentation

output: 
	mkdir -p image_test_nn

segmentation: segmentation.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f segmentation
	rm -f segmentation.o
	${RM} *.bmp
