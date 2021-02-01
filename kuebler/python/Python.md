# Controller

The controller software is not as far as the c++ variant.
A simple c++ Robot Wrapper was created for the KINOVA API that can be used to create a python wrapper.

### Wrapping KINOVA API

A more complex tutorial on python wrapping with swig can be found [here](https://www.geeksforgeeks.org/wrapping-cc-python-using-swig-set-1/).

1. Install compiler and wrapping tools

	compiler installation
	```console
	user@computer: sudo apt install g++
	```

	Phython development tools
	```console
	user@computer: apt-get install python-dev
	```

	Wrapping tool
	```console
	user@computer: apt install swig
	```

2. Generate wrapper configuration file
	Enter the directory where the phython wrapper needs to be created.

	Use swig to generate wrapper configuration file.
	Output type: -python
	Input type: -c++
	Source directory of c++ files: -I<path/to/sourcefiles>
	Output config file: <path/to/new/config_file.i>
	```console
	user@computer: swig -python -c++ -I../src KinovaJaco2.i
	```

3. Compile and wrapp
	Enter the directory where the phython wrapper needs to be created.

	Use swig to generate wrapper configuration file.
	Output type: -python
	Input type: -c++
	Source directory of c++ files: -I<path/to/sourcefiles>
	Output config file: <path/to/new/config_file.i>
	```console
	user@computer: g++ -Wall -fPIC -c ../src/KinovaJaco2*.c* ./KinovaJaco2*.c* -I/usr/include/python2.7 -I/usr/include -I../include -I../src -L/usr/lib -L../lib -lpython2.7 -ldl
	```
	It is important to include all files. ../src/KinovaJaco2*.c* ./KinovaJaco2*.c* will also include the swig generated files because of the wild card placement. In order for g++ to compile a python output, -lpython2.7  from -I/usr/include/python2.7 has to be included as well.

4. Link files
	The compiler result has to be linked to create a shared object file for python to use.
	```console
	user@computer: g++ -shared -fPIC KinovaJaco2*.o -o _KinovaJaco2.so
	```

# Camera

The camera can be operated only using python packages. The setup is therefore relatively easy and only requires the following packages.

### Required packages

#### Python Package manager
```console
user@computer: sudo apt install python-pip
```

#### Numpy for mathematical operations
```console
user@computer: pip install numpy
```

#### OpenCV for computer vision operations
```console
user@computer: pip install opencv-python
```