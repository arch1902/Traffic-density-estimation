## Using makefile
```make all```/ ```make```

To compile your .cpp files

```make clean```

To remove the generated .o files

```make help``` / ```make {any other string}```

To print the documentation of make file

## Using final_task2.o

```./final_task2.o {input_file}```

Example:

```./final_task2.o trafficvideo.mp4```

## User Interface

* On running ````./final_task2.o {input_file}```` command, the intensity of the dynamic and queue density will start printing on the console.
* You will also see the perpective frame of the input image and the live plots of dynamic and queue density. 
* The format of printing on console is ```framenum, queue density, dynamic density``` .  
* The intensity we obtained while running our code are stored in "out.txt" file and the final plot in "out.png" image.
* Image "bg.png" contains the empty backgroud image, please make sure that it is present in the main directory while running the code. 



### Error Handling

* If the input file is not present in the directory it will not proceed further in the code.
