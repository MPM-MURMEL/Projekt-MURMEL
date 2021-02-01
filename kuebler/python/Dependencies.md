# OpenCV

An open source computer vision library.
More information on OpenCV can be found [here](https://opencv.org/).
And for usage with depth cameras [here](https://docs.opencv.org/3.3.0/d7/d6f/tutorial_kinect_openni.html).

1. Open a Terminal and install tools
	
	```console
	user@computer: sudo apt-get install git cmake libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev 
	```


2. Format external drive 
	
	This step is only necessary if the system does not have enought memory to build opencv
	The drive must be in ext4 format to support symbolic linking.
	
	```console
	user@computer: sudo mkfs.ext4 /dev/<sda-name> 
	```


3. Create and enter git directory
	
	Clone the git repository to the external drive if step 2. applied.

	```console
	user@computer: mkdir <git-directory>
	user@computer: cd <git-directory>
	```

4. Clone OpenCV repository
	
	```console
	user@computer: git clone https://github.com/opencv/opencv.git
	```

5. Create and enter build directory in opencv directory
	
	```console
	user@computer: cd opencv
	user@computer: mkdir build
	user@computer: cd build
	```

6. Generate makefile with cmake
	
	Precompiled headers can be disabled if the system does not have enpught memory.
	
	```console
	user@computer: sudo cmake -D ENABLE_PRECOMPILED_HEADERS=OFF -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
	```

7. Build OpenCV
	
	This step might take a while.

	```console
	user@computer: make -j4
	```

8. Install OpenCV
	
	```console
	user@computer: make install
	```


# OpenNi

1. Install dependancies
 	
	```console
	user@computer: sudo apt-get install freeglut3 freeglut3-dev
	```

2. Download OpenNI for Orbbec Persee 
	
	```console
	user@computer: wget http://www.orbbec3d.net/Tools_SDK_OpenNI/2-Linux.zip
	```

3. Unzip and enter downloaded directory
	
	```console
	user@computer: unzip 2-Linux.zip
	user@computer: cd 2-Linux
	```

4. Unzip sub directory and enter location
	
	```console
	user@computer: unzip OpenNI-Linux-x64-2.3.zip -d ~/OpenNi
	user@computer: cd ~/OpenNi/OpenNi-Linux-x64-2.3
	```

4. Install
	
	```console
	user@computer: sudo chmod a+x install.sh
	user@computer: sudo ./install.sh
	user@computer: source OpenNIDevEnvironment
	```

# Eigen

A open source template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
More information on Eigen can be found [here](http://eigen.tuxfamily.org/index.php?title=Main_Page#Overview).

Installation via terminal:
	
```console
user@computer: sudo apt-get install libeigen3-dev
```

# JsonCpp

A Json parser for c++.
More information on JsonCpp can be found [here](https://en.wikibooks.org/wiki/JsonCpp).

Installation via terminal:
```console
user@computer: sudo apt-get install libjsoncpp-dev
```