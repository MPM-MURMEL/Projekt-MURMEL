SDIR=./src
BDIR=./bin
EDIR=./example
DDIR=./doc

IDIR=-I./include -I/usr/include -I/usr/local/include -I/usr/local/include/opencv4
LDIR=-L./lib -L/usr/lib -L/usr/local/lib -L/usr/local/lib -L./lib/OpenNI2/Drivers/

libs= -ldl -ljsoncpp -lopencv_core -lopencv_highgui -lopencv_videoio -lopencv_imgproc -lopencv_imgcodecs -lOniFile -lOpenNI2 -lorbbec -lPSLink


RPATH = -Wl,-rpath,/usr/local/lib/
RPATH += -Wl,-rpath,lib
RPATH += -Wl,-rpath,lib/OpenNI2/Drivers/

CC=g++
CFLAGS= -std=c++11 $(RPATH) $(IDIR) $(LDIR) $(libs)


all: clean main


clean:
	rm -f -r $(BDIR)/*
	rm -f -r $(DDIR)/html
	rm -f -r $(DDIR)/latex
	rm -f -r $(DDIR)/xml


documentation:
	cd $(DDIR); doxygen Documentation.conf;


main: TCPServer.o KeyholeDetection.o
	$(CC)  $(EDIR)/Camera.cpp $(BDIR)/*.o -o $(BDIR)/Camera $(CFLAGS)


example: TCPServer.o KeyholeDetection.o
	$(CC)  $(EDIR)/TEST_TCPServer.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer $(CFLAGS)
	$(CC)  $(EDIR)/TEST_TCPServer_KinovaJaco.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer_KinovaJaco $(CFLAGS)
	$(CC)  $(EDIR)/TEST_TCPServer_Json.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPServer_Json $(CFLAGS)
	$(CC)  $(EDIR)/TEST_OpenCV.cpp $(BDIR)/*.o -o $(BDIR)/TEST_OpenCV $(CFLAGS)
	$(CC)  $(EDIR)/TEST_OpenNI.cpp $(BDIR)/*.o -o $(BDIR)/TEST_OpenNI $(CFLAGS)


TCPServer.o: $(SDIR)/TCPServer.cpp
	$(CC) -c $(SDIR)/TCPServer.cpp -o $(BDIR)/TCPServer.o $(CFLAGS)


KeyholeDetection.o: $(SDIR)/KeyholeDetection.cpp
	$(CC) -c $(SDIR)/KeyholeDetection.cpp -o $(BDIR)/KeyholeDetection.o $(CFLAGS)
