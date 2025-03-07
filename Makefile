# Define the compiler
CC = gcc

# Define the source and target
SRCS = ndn.c auxiliar_functions.c head.h
TARGET = ndn

# Default target
all:
	$(CC) $(SRCS) -o $(TARGET)

# Clean target
clean:
	rm -f $(TARGET)