#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include <eigen3/Eigen/Dense>
#include <jsoncpp/json/json.h>

#include "../src/KinovaJaco2.hpp"
#include "../src/TCPClient.hpp"


using namespace std;


int main(int argc, char* argv[])
{
	cout << "[INFO] Starting TCP Client..." << endl;
	
	while(true)
	{
		//TCPClient client("127.0.0.1", 8080); 
		TCPClient client("192.168.100.20", 8080); 
		
		if (client.isConnected())
		{
			cout << "[INFO] Connected" << endl;

			//check if still connected to server
			while(client.isConnected())
			{

				//recheive msg from server
				string msg = client.receive(256);
				cout << "msg: " << msg << endl; 

				// json tools for parsing
				Json::Value root;
				Json::Reader reader;

				Json::Value dx = 0;
				Json::Value dy = 0;

				// check if message can be parsed
				if(reader.parse(msg, root))
				{
					// dx and dy must be in the json string defined on the server side
					dx = root["dx"];
					dy = root["dy"];
				}
				else
				{
					cout << "[WARNING] Cannot parse Json message" << endl;
					continue;
				}
			

				cout << "dx: " << dx << endl;
				cout << "dy: " << dy << endl;

				
				usleep(1000);
			}

			cout << "[INFO] Connection lost" << endl;
		}

		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
	
	return 0;
}



