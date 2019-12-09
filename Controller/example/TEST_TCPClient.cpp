#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include "../src/TCPClient.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	
	cout << "[INFO] Starting TCP Client..." << endl;

	while(true)
	{
		// start tcp client
		//TCPClient client("127.0.0.1", 8080); 
		TCPClient client("192.168.100.20", 8080); 

		cout << "[INFO] Connected" << endl;

		// check if client is connected to server
		while(client.isConnected())
		{
			// receive string
			string msg = client.receive(256);
			cout << msg << endl;
			usleep(10000);
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
}
