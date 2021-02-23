CFLAGS: -g -Wall -pthread 
CC = g++

all: com.cpp
	$(CC) $(CFLAGS) com.cpp -o com -std=c++11 `pkg-config --cflags --libs opencv4`
clean:
	$(RM) com 
