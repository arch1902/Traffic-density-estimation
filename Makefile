all: myprog.cpp
    gcc -g -Wall -o myprog myprog.cpp
clean:
 $(RM) myprog