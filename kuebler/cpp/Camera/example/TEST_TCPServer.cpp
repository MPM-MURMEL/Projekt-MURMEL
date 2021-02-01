#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include "../src/TCPServer.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	
	cout << "[INFO] Starting TCP Server..." << endl;

	while(true)
	{
		// start tcp server
		TCPServer server(8080); 

		cout << "[INFO] Connected" << endl;
		
		// counter variable for message
		int counter = 0;

		// check if a client is connected
		while(server.isConnected())
		{
			//send counter as string
			server.send(to_string(counter++)); 
			usleep(1000);
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
}
