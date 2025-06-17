C_SOURCES = $(wildcard matrix/*.c neural/*.c util/*.c *.c)
HEADERS = $(wildcard matrix/*.h neural/*.h util/*.h *.h)
OBJ = ${C_SOURCES:.c=.o}
CFLAGS =

MAIN = main
CC = gcc
LINKER = /usr/bin/ld


main: ${OBJ}
	${CC} ${CFLAGS} $^ -o $@ -lm -lSDL2 -lSDL2_image

# Generic rules
%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@ -lm -lSDL2 -lSDL2_image

clean:
	rm matrix/*.o *.o neural/*.o util/*.o ${MAIN}
