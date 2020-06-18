SDIR=./src
IDIR=./include
LDIR=./lib
BDIR=./bin
EDIR=./example
DDIR=./doc

libs= -ldl -ljsoncpp

CC=g++
CFLAGS=-I/usr/include -I$(IDIR) -L/usr/lib -L$(LDIR) $(libs)


all: clean main


clean:
	rm -f -r $(BDIR)/*
	rm -f -r $(DDIR)/html
	rm -f -r $(DDIR)/latex
	rm -f -r $(DDIR)/xml


documentation:
	cd $(DDIR); doxygen Documentation.conf; cd ..


main: KinovaJaco2.o TCPClient.o PIDController.o ExponentialFilter.o
	$(CC) $(EDIR)/Controller.cpp $(BDIR)/*.o -o $(BDIR)/Controller $(CFLAGS)


example: KinovaJaco2.o TCPClient.o PIDController.o ExponentialFilter.o
	$(CC) $(EDIR)/TEST_TCPClient.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPClient $(CFLAGS)
	$(CC) $(EDIR)/TEST_KinovaJaco.cpp $(BDIR)/*.o -o $(BDIR)/TEST_KinovaJaco $(CFLAGS)
	$(CC) $(EDIR)/TEST_TCPClient_KinovaJaco.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPClient_KinovaJaco $(CFLAGS)
	$(CC) $(EDIR)/TEST_TCPClient_Jason.cpp $(BDIR)/*.o -o $(BDIR)/TEST_TCPClient_Jason $(CFLAGS)
	$(CC) $(EDIR)/TEST_PIDController.cpp $(BDIR)/*.o -o $(BDIR)/TEST_PIDController $(CFLAGS)


ExponentialFilter.o: $(SDIR)/ExponentialFilter.cpp
	$(CC) -c $(SDIR)/ExponentialFilter.cpp -o $(BDIR)/ExponentialFilter.o $(CFLAGS)


PIDController.o: $(SDIR)/PIDController.cpp
	$(CC) -c $(SDIR)/PIDController.cpp -o $(BDIR)/PIDController.o $(CFLAGS)


KinovaJaco2.o: $(SDIR)/KinovaJaco2.cpp
	$(CC) -c $(SDIR)/KinovaJaco2.cpp -o $(BDIR)/KinovaJaco2.o $(CFLAGS)


TCPClient.o: $(SDIR)/TCPClient.cpp
	$(CC) -c $(SDIR)/TCPClient.cpp -o $(BDIR)/TCPClient.o $(CFLAGS)

