  all: subtask.cpp
 	  g++ subtask1.cpp -o subtask -pthread -std=c++11 `pkg-config --cflags --libs opencv`

  clean: 
	  $(RM) myprog