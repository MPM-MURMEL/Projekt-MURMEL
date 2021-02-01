#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <string>

#include "../src/TCPServer.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	// set terminal mode
	// allows for input without return
	// changes the terminal style, not usable for reading purposes, only for command input
	system("stty raw");


	cout << "[INFO] Starting TCP Server..." << endl;

	while(true)
	{
		// start tcp server
		TCPServer server(8080); 

		cout << "[INFO] Connected" << endl;

		// check if a client is connected
		while(server.isConnected())
		{

			// parse keyboard keys to message keywords
			char c = getchar();
			string msg;
			if(c == 'w') 
			{
				msg = "UP";
			}
			else if(c == 's') 
			{
				msg = "DOWN";
			}
			else if(c == 'a') 
			{
				msg = "LEFT";
			}
			else if(c == 'd') 
			{
				msg = "RIGHT";
			}
			else if(c == 'd') 
			{
				msg = "RIGHT";
			}
			else if(c == 'q') 
			{
				// quit test if 'q' is pressed
				exit(0);
			}
		
			cout << msg << endl;

			// send message
			server.send(msg); 

			usleep(1000);
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
}
