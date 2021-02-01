# Installation


1. Open a Terminal and install make
	
	```console
	user@computer: sudo apt-get install make
	```


# Usage

1. Go to your project home

	```console
	user@computer: cd /path/to/project/home
	```

2. Create a build directory

	```console
	user@computer: mkdir <build-directory>
	```

3. Write a make file

	```Makefile
	# source directory
	SDIR=./src (.c .cpp)

	# binary or executable directory
	BDIR=./bin

	# example source direcotry (.c .cpp)
	EDIR=./example

	# documentation directory for doxygen
	DDIR=./doc

	# include directory (.h .hpp)
	IDIR=-I./include -I/usr/include -I/usr/local/include -I/usr/local/include/opencv4

	# librarie directorie (.o .so)
	LDIR=-L./lib -L/usr/lib -L/usr/local/lib -L/usr/local/lib -L./lib/OpenNI2/Drivers/

	# libraries to look for in librarie directory
	libs= -ldl -ljsoncpp -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lOniFile -lOpenNI2 -lorbbec -lPSLink

	# tells the linker to include librariy paths
	# this means LD_LIBRARY_PATH does not need to be modifyed before executen
	RPATH = -Wl,-rpath,/usr/local/lib/
	RPATH += -Wl,-rpath,lib
	RPATH += -Wl,-rpath,lib/OpenNI2/Drivers/

	# compiler
	CC=g++

	# compilter options and all paths required
	CFLAGS= -std=c++11 $(RPATH) $(IDIR) $(LDIR) $(libs)


	# make options structure
	# <tag>: <other make tags to execute before>
	#      <command 1>
	#	   <command 3>
	#
	# can be executeded using: make <tag>


	all: clean main

	# removes all generated files
	clean:
		rm -f -r $(BDIR)/*
		rm -f -r $(DDIR)/html
		rm -f -r $(DDIR)/latex
		rm -f -r $(DDIR)/xml

	# executes documentation if doxygen was setup
	documentation:
		cd $(DDIR); doxygen Camera.conf; cd ..

	# compiles main program
	main: TCPServer.o KeyholeDetection.o
		$(CC)  $(EDIR)/Camera.cpp $(BDIR)/*.o -o $(BDIR)/Camera $(CFLAGS)

	#compiles test programs
	example: TCPServer.o KeyholeDetection.o
		$(CC)  $(EDIR)/TEST_TCPServer.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer $(CFLAGS)
		$(CC)  $(EDIR)/TEST_TCPServer_KinovaJaco.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer_KinovaJaco $(CFLAGS)
		$(CC)  $(EDIR)/TEST_TCPServer_Json.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer_Json $(CFLAGS)
		$(CC)  $(EDIR)/TEST_OpenCV.cpp $(BDIR)/*.o -o $(BDIR)/TEST_OpenCV $(CFLAGS)

	# precompiles a class to a shares object library (.so)
	TCPServer.o: $(SDIR)/TCPServer.cpp
		$(CC) -c $(SDIR)/TCPServer.cpp -o $(BDIR)/TCPServer.o $(CFLAGS)

	# precompiles a class to a shares object library (.so)
	KeyholeDetection.o: $(SDIR)/KeyholeDetection.cpp
		$(CC) -c $(SDIR)/KeyholeDetection.cpp -o $(BDIR)/KeyholeDetection.o $(CFLAGS)
	```

4. Build Project

	```console
	user@computer: make <tag>
	```
	The tag corresponse to the tags in the makefile.
	For example: 
	```console
	user@computer: make documentation
	```
	would generate the documentation as define in the file and
	```console
	user@computer: make clean
	```
	would remove all generated files.
	The tag all will be used as default
	```console
	user@computer: make
	```

# Output 

If ALL previously described instructions were followed
the resulting structure should resemble the following file tree.

```
/path/to/project/home/
│
└───<build-directory>
│   │   <bin-file>
│
└───<lib-directory>
└───<example-directory>
└───<include-director>
└───<source-directory>
└───<documentation-directory>
```
