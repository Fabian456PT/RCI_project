# Define the compiler
CC = gcc

# Define the source and target
SRCS = ndn.c auxiliar_functions.c command_lines.c connections.c head.h 
TARGET = ndn

# Default target
all:
	$(CC) $(SRCS) -g -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)