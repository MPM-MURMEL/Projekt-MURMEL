#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <sstream>

#include <jsoncpp/json/json.h>

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
			// define x and y as variables
			double x = 10;
			double y = 20;

			// integrate variables into json string
			stringstream ss;
			ss << "{ \"dx\": " << x << ", \"dy\": " << y << "}";

			//send counter as string
			server.send(ss.str()); 
			usleep(1000);
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
}
