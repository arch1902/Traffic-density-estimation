## Using makefile
```make all```/ ```make```

To compile your .cpp files

```make clean```

To remove the generated .o files

```make help```

To print the documentation of make file

## Using com.o

```./com.o {input_file}```

Example:

```./com.o empty.jpg```

## User Interface

* On running the ````./com.o {input_file}```` command you will see a image pop-up on your screen. You will have to select the four corners of the road under consideration. This can be done in any order. You will also see the coordinates of the points you select on this Window.
* Then press any key to obtain the cropped Image.
* Another key press will close all the open windows and terminate the program.
* The prespective and cropped images will be saved in the directory.


### Error Handling

* If the image file is not found in the same directoy or is unreadable it will stop the program with a corresponding error.
* If the key is pressed before choosing four points it will also terminate the program. 
