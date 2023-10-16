SRC=src
OBJ=build
CC=gcc
CFLAGS=-I$(SRC) -lc -g

$(OBJ)/FileCopy.o: $(SRC)/FileCopy.c
	mkdir -p $(OBJ)
	$(CC) -c $(SRC)/FileCopy.c -o $(OBJ)/FileCopy.o

FileCopy: $(OBJ)/FileCopy.o
	$(CC) -o FileCopy $(OBJ)/FileCopy.o $(CFLAGS)

clean:
	rm -rf build FileCopy
