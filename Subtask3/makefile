## If the user enters a wrong command
.DEFAULT:
	@echo "This is a documentation of make file"
	@echo  "'make'/'make all' is used to build the executable file"
	@echo "'make clean' is used to remove the compiled file"
	@echo "'make help' is used to call this documentation"

# Define compiler
CC = g++

 # compiler flags:
 CFLAGS = -g -Wall

# Cpp flags
CONFIG = `pkg-config --cflags --libs opencv4`
CPPFLAGS = -lpthread -std=c++11

# Executable file
TARGET = final_task2

all: $(TARGET).cpp
	@echo "Building all files ....."
	$(CC) $(TARGET).cpp -o $(TARGET).o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	@echo "Your code has been compiled Successfully!!."
	@echo "Run './final_task2.o <Video Name>' to start the program"
clean:
	@echo "Cleaning all files"
	$(RM) $(TARGET).o 

help: 
	@echo "This is a documentation of make file"
	@echo  "'make'/'make all' is used to build the executable file"
	@echo "'make clean' is used to remove the compiled file"
	@echo "'make help' is used to call this documentation"