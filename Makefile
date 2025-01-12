# Makefile to compile server.c using gcc

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g  # -Wall for all warnings, -g for debugging symbols

# Output executable file name
OUTPUT = server

# Source file
SRC = server.c

# Object file (generated from the source file)
OBJ = server.o

# Default target
all: $(OUTPUT)

# Rule to compile server.c into server.o
$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

# Rule to link server.o to create the executable
$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(OUTPUT)

# Clean up generated files
clean:
	rm -f $(OBJ) $(OUTPUT)

# Target to debug the build process
debug:
	$(CC) $(CFLAGS) -DDEBUG $(SRC) -o $(OUTPUT)