#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include "../src/KinovaJaco2.hpp"
#include "../src/TCPClient.hpp"

#include <eigen3/Eigen/Dense>


using namespace std;


int main(int argc, char* argv[])
{
	// initialize Jaco2
	cout << "[INFO] Starting KinovaJaco2..." << endl;
	KinovaJaco2 robot = KinovaJaco2();

	// calibrate Jaco2
	cout << "[INFO] Calibrate robot..." << endl;
	robot.initHome();

	// send to retracted position
	cout << "[INFO] Retract robot..." << endl;
	robot.sendRetractedTrajectory();

	// send to home position
	cout << "[INFO] Enable robot..." << endl;
	robot.sendHomeTrajectory();

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

			double x = 0;
			double y = 0;

			// receive string message from server
			string msg = client.receive(256);
			cout << "msg: " << msg << endl; 

			// string message is parsed to speeds
			// "UP" -> x=0.1
			// "DOWN" -> x=-0.1
			if (msg.compare("UP") == 0)
			{
				cout << "UP" << endl;
				x = 0.1;
			}
			else if (msg.compare("DOWN") == 0)
			{
				cout << "DOWN" << endl;
				x = -0.1;
			}
			else if (msg.compare("LEFT") == 0)
			{
				cout << "LEFT" << endl;
				y = 0.1;
			}
			else if (msg.compare("RIGHT") == 0)
			{
				cout << "RIGHT" << endl;
				y = -0.1;
			}
			else
			{
				//skip iteration
				continue;
			}
		
			cout << "x: " << x << endl;
			cout << "y: " << y << endl;

			// trajectory command struct
			TrajectoryPoint commandIn;
			commandIn.InitStruct();
			commandIn.Position.HandMode = HAND_NOMOVEMENT; 
			commandIn.Position.Type = CARTESIAN_VELOCITY;

			// speed in every direction/orientation
			commandIn.Position.CartesianPosition.X = x;
			commandIn.Position.CartesianPosition.Y = y;
			commandIn.Position.CartesianPosition.Z = 0; 
			commandIn.Position.CartesianPosition.ThetaX = 0;
			commandIn.Position.CartesianPosition.ThetaY = 0;
			commandIn.Position.CartesianPosition.ThetaZ = 0;

			//get the current Cartesian position
			CartesianPosition CartPos;
			robot.getCartesianPosition(CartPos);

			// apply the right transformation to convert the speed command in the gripper reference frame
			TrajectoryPoint commandOut = robot.convertReferenceFrame(CartPos, commandIn);

			// send the command
			robot.sendScheduledTrajectory(commandOut);
	
			usleep(1000);
		}

		cout << "[INFO] Connection lost" << endl;
		usleep(500000);
		cout << "[INFO] Reconnecting..." << endl;
	}
	
	return 0;
}



