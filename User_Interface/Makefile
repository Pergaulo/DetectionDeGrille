CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

all: User_Interface

User_Interface: U_I.c
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f User_Interface
	rm -f test_blackandwhite.jpg
	rm -f rotation.jpg
	rm -f cropped.jpg
	rm -f line.jpg
	rm -f segment.jpg
