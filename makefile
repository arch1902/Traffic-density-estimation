CFLAGS: -g -Wall -pthread -std=c++11
CC = g++

all: com.cpp
	$(CC) $(CFLAGS) com.cpp -o com `pkg-config --cflags --libs opencv4`
clean:
	$(RM) com 