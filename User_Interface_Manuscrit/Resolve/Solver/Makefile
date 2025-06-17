# Makefile

CC = gcc -fsanitize=address
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

OBJ = solver.o
DEP = ${OBJ:.o=.d}

all: solver

solver: ${OBJ}

clean:
	${RM} ${OBJ} ${DEP} solver grid_00.result

-include ${DEP}

# END
