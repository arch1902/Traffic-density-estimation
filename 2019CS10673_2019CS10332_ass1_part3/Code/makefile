## If the user enters a wrong command
.DEFAULT:
	@echo "This is a documentation of make file"
	@echo  "'make'/'make all' is used to build the executable file"
	@echo "'make clean' is used to remove the compiled file"
	@echo "'make help' is used to call this documentation"

# Define compiler
CC = g++

 # compiler flags:
 CFLAGS = -g -Wall -Wno-int-to-void-pointer-cast

# Cpp flags
CONFIG = `pkg-config --cflags --libs opencv4`
CPPFLAGS = -lpthread -std=c++11

# Executable file

all: 
	@echo "Building all files ....."
	$(CC) baseline.cpp -o baseline.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) method1.cpp -o method1.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) method2.cpp -o method2.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) method3.cpp -o method3.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) method4.cpp -o method4.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) dense_of.cpp -o dense.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)
	$(CC) sparse_of.cpp -o sparse.o $(CFLAGS) $(CPPFLAGS) $(CONFIG)

	@echo "Your code has been compiled Successfully!!."
	@echo "Run './<Method>.o <Video Name>' to start the program"
clean:
	@echo "Cleaning all files"
	$(RM) $(TARGET).o 

help: 
	@echo "This is a documentation of make file"
	@echo  "'make'/'make all' is used to build the executable file"
	@echo "'make clean' is used to remove the compiled file"
	@echo "'make help' is used to call this documentation"
