#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include <jsoncpp/json/json.h>
#include <eigen3/Eigen/Dense>

#include "../src/KinovaJaco2.hpp"
#include "../src/TCPClient.hpp"
#include "../src/PIDController.hpp"
#include "../src/ExponentialFilter.hpp"


using namespace std;


// offset for servoing
const int offset_x = 15;
const int offset_y = -55; //Orbbec 0
const int offset_z = -150; //Orbbec -130

// x and y controll error threshold
const int x_y_thresh = 10;

// dz distance to trash can thresh
const double dz_speed = 0.2;


enum OperationState 
{
	approaching,
	insertion,
	opening,
	extracting,
	finished
};

OperationState op_state;

int main(int argc, char* argv[])
{
	
	// initialize Jaco2
	cout << "[INFO] Starting KinovaJaco2..." << endl;
	KinovaJaco2 robot = KinovaJaco2();

	// calibrate Jaco2
	cout << "[INFO] Calibrate robot..." << endl;
	//robot.initHome();

	// send to retracted position
	//cout << "[INFO] Retract robot..." << endl;
	//robot.sendRetractedTrajectory();

	// send to home position
	cout << "[INFO] Enable robot..." << endl;
	robot.sendHomeTrajectory();


	// set default state
	op_state = approaching;

	while(true)
	{
			
		if (op_state == approaching)
		{
			usleep(2000000);

			// start tcp client
			cout << "[INFO] Starting TCP Client..." << endl;
			TCPClient client("192.168.100.20", 8080); 
			
			if (client.isConnected())
			{
				cout << "[INFO] Connected" << endl;
			
				ExponentialFilter f_x = ExponentialFilter(0.1);
				ExponentialFilter f_y = ExponentialFilter(0.1);
				ExponentialFilter f_r = ExponentialFilter(0.1);

				// create pid controllers for x and y direction
				PIDController p_x = PIDController(0.002,0,0);
				PIDController p_y = PIDController(0.002,0,0);
				PIDController p_z = PIDController(0.1,0,0);

				// send tracking command to camera
				client.send("tracking");

				// check if client is connected to server
				while(client.isConnected())
				{
					double dx = 0;
					double dy = 0;
					double dz = 0;

					//recheive msg from server
					string msg = client.receive(256);

					// json tools for parsing
					Json::Value root;
					Json::Reader reader;

					// check if message can be parsed
					if(reader.parse(msg, root))
					{
						// dx and dy must be in the json string defined on the server side
						try
						{
							dx = root["dx"].asDouble();
							dy = root["dy"].asDouble();

							// needs to be inverted 
							// zero starts at trash can surface not camera lense
							dz = -1 * root["dz"].asDouble();
						} 
						catch (...)
						{
							cout << "[WARNING] Cannot cast Json message values" << endl;
							continue;
						}
						
					}
					else
					{
						cout << "[WARNING] Cannot parse Json message" << endl;
						continue;
					}

					// calculate filtered values
					dx = f_x.calculate(dx);
					dy = f_y.calculate(dy);
					dz = f_r.calculate(dz);

					// calculate controll values
					// speed needs to be influenced by distance
					dx = p_x.calculate(offset_x - dx);
					dy = p_y.calculate(offset_y - dy);
					dz = p_y.calculate(offset_z - dz);

					cout << "dx: " << dx << endl;
					cout << "dy: " << dy << endl;
					cout << "dz: " << dz << endl;

					// change op_state
					if (dz <= 0)
					{	
						op_state = insertion;
						break;
					}



					// only move forward if robot is centered on keyhole
					if (!((abs(dx) < x_y_thresh && abs(dy) < x_y_thresh)))
					{
						dz = 0;		
					}
					else if(dz < 0.2)
					{
						// speeds below 0.1 are not usable for servoing
						dz = 0.2;	
					}



					// trajectory command struct
					TrajectoryPoint target_pos;
					target_pos.InitStruct();
					target_pos.Position.HandMode = HAND_NOMOVEMENT; 
					target_pos.Position.Type = CARTESIAN_VELOCITY;

					// speed in every direction/orientation
					target_pos.Position.CartesianPosition.X = dx;
					target_pos.Position.CartesianPosition.Y = dy;
					target_pos.Position.CartesianPosition.Z = dz; 
					target_pos.Position.CartesianPosition.ThetaX = 0;
					target_pos.Position.CartesianPosition.ThetaY = 0;
					target_pos.Position.CartesianPosition.ThetaZ = 0;

					//get the current Cartesian position
					CartesianPosition current_pos;
					robot.getCartesianPosition(current_pos);

					// apply the right transformation to convert the speed command in the gripper reference frame
					// orbbec and refframe are misoriented by 90deg around z
					TrajectoryPoint command_pos = robot.convertReferenceFrame(current_pos, target_pos);

					// send the command
					robot.sendScheduledTrajectory(command_pos);
			
					//usleep(1000);
				}

				cout << "[INFO] Connection lost" << endl;
			}

			//usleep(100000);
		}
		else if(op_state == insertion)
		{
			cout << "[INFO] Insert Key" << endl;

			// trajectory command struct
			TrajectoryPoint target_pos;

			target_pos.InitStruct();
			target_pos.Position.HandMode = HAND_NOMOVEMENT; 
			target_pos.Position.Type = CARTESIAN_POSITION;

			// speed in every direction/orientation
			target_pos.Position.CartesianPosition.X = 0; // orbbec -0.017
			target_pos.Position.CartesianPosition.Y = 0; // orbbec -0.09
			target_pos.Position.CartesianPosition.Z = 0.09; // orbbec 0
			target_pos.Position.CartesianPosition.ThetaX = 0;
			target_pos.Position.CartesianPosition.ThetaY = 0;
			target_pos.Position.CartesianPosition.ThetaZ = 0;

			//get the current Cartesian position
			CartesianPosition current_pos;
			robot.getCartesianCommand(current_pos);

			// apply the right transformation to convert the speed command in the gripper reference frame
			// orbbec and refframe are misoriented by 90deg around z
			TrajectoryPoint command_pos = robot.convertReferenceFrame(current_pos, target_pos);

			command_pos.Position.CartesianPosition.X += current_pos.Coordinates.X;
			command_pos.Position.CartesianPosition.Y += current_pos.Coordinates.Y;
			command_pos.Position.CartesianPosition.Z += current_pos.Coordinates.Z;
			command_pos.Position.CartesianPosition.ThetaX += current_pos.Coordinates.ThetaX;
			command_pos.Position.CartesianPosition.ThetaY += current_pos.Coordinates.ThetaY;
			command_pos.Position.CartesianPosition.ThetaZ += current_pos.Coordinates.ThetaZ;

			for (int i=0; i<100; i++)
			{
				// send the command
				robot.sendScheduledTrajectory(command_pos);
				//usleep(1000);
			}

			
			/*

			// trajectory command struct
			target_pos.InitStruct();
			target_pos.Position.HandMode = HAND_NOMOVEMENT; 
			target_pos.Position.Type = CARTESIAN_POSITION;

			// speed in every direction/orientation
			target_pos.Position.CartesianPosition.X = 0;
			target_pos.Position.CartesianPosition.Y = 0;
			target_pos.Position.CartesianPosition.Z = 0.04; 
			target_pos.Position.CartesianPosition.ThetaX = 0;
			target_pos.Position.CartesianPosition.ThetaY = 0;
			target_pos.Position.CartesianPosition.ThetaZ = 0;

			robot.getCartesianCommand(current_pos);

			// apply the right transformation to convert the speed command in the gripper reference frame
			// orbbec and refframe are misoriented by 90deg around z
			command_pos = robot.convertReferenceFrame(current_pos, target_pos);

			command_pos.Position.CartesianPosition.X += current_pos.Coordinates.X;
			command_pos.Position.CartesianPosition.Y += current_pos.Coordinates.Y;
			command_pos.Position.CartesianPosition.Z += current_pos.Coordinates.Z;
			command_pos.Position.CartesianPosition.ThetaX += current_pos.Coordinates.ThetaX;
			command_pos.Position.CartesianPosition.ThetaY += current_pos.Coordinates.ThetaY;
			command_pos.Position.CartesianPosition.ThetaZ += current_pos.Coordinates.ThetaZ;


			for (int i=0; i<100; i++)
			{
				// send the command
				robot.sendScheduledTrajectory(command_pos);
				//usleep(1000);
			}
			*/

			// change op_state after approaching

			op_state = opening;
		}
		else if (op_state == opening)
		{
			//usleep(1000000);
			cout << "[INFO] Opening" << endl;


			// get current angular position from robot
			AngularPosition current_angles;
			robot.getAngularCommand(current_angles);


			// definition of retracted position in angular mode
			TrajectoryPoint command_angles;
			command_angles.InitStruct();
			command_angles.Position.Type = ANGULAR_POSITION;

			command_angles.Position.Actuators.Actuator1 = current_angles.Actuators.Actuator1;
			command_angles.Position.Actuators.Actuator2 = current_angles.Actuators.Actuator2;
			command_angles.Position.Actuators.Actuator3 = current_angles.Actuators.Actuator3;
			command_angles.Position.Actuators.Actuator4 = current_angles.Actuators.Actuator4;
			command_angles.Position.Actuators.Actuator5 = current_angles.Actuators.Actuator5;
			command_angles.Position.Actuators.Actuator6 = current_angles.Actuators.Actuator6 + 90;
			command_angles.Position.Fingers.Finger1 = 0;
			command_angles.Position.Fingers.Finger2 = 0;
			command_angles.Position.Fingers.Finger3 = 0;

			for (int i=0; i<100; i++)
			{
				// send the command
				robot.sendScheduledTrajectory(command_angles);
				//usleep(1000);
			}

			op_state = extracting;
		}
		else if (op_state == extracting)
		{

			//usleep(1000000);
			cout << "[INFO] Extracting key from keyhole" << endl;

			// get current angular position from robot
			AngularPosition current_angles;
			robot.getAngularCommand(current_angles);


			// definition of retracted position in angular mode
			TrajectoryPoint command_angles;
			command_angles.InitStruct();
			command_angles.Position.Type = ANGULAR_POSITION;

			command_angles.Position.Actuators.Actuator1 = current_angles.Actuators.Actuator1;
			command_angles.Position.Actuators.Actuator2 = current_angles.Actuators.Actuator2;
			command_angles.Position.Actuators.Actuator3 = current_angles.Actuators.Actuator3;
			command_angles.Position.Actuators.Actuator4 = current_angles.Actuators.Actuator4;
			command_angles.Position.Actuators.Actuator5 = current_angles.Actuators.Actuator5;
			command_angles.Position.Actuators.Actuator6 = current_angles.Actuators.Actuator6 - 90;
			command_angles.Position.Fingers.Finger1 = 0;
			command_angles.Position.Fingers.Finger2 = 0;
			command_angles.Position.Fingers.Finger3 = 0;

			for (int i=0; i<100; i++)
			{
				// send the command
				robot.sendScheduledTrajectory(command_angles);
				//usleep(1000);
			}


			// trajectory command struct
			TrajectoryPoint target_pos;
			target_pos.InitStruct();
			target_pos.Position.HandMode = HAND_NOMOVEMENT; 
			target_pos.Position.Type = CARTESIAN_POSITION;

			// speed in every direction/orientation
			target_pos.Position.CartesianPosition.X = 0;
			target_pos.Position.CartesianPosition.Y = 0;
			target_pos.Position.CartesianPosition.Z = -0.15; 
			target_pos.Position.CartesianPosition.ThetaX = 0;
			target_pos.Position.CartesianPosition.ThetaY = 0;
			target_pos.Position.CartesianPosition.ThetaZ = 0;

			//get the current Cartesian position
			CartesianPosition current_pos;
			robot.getCartesianCommand(current_pos);

			// apply the right transformation to convert the speed command in the gripper reference frame
			// orbbec and refframe are misoriented by 90deg around z
			TrajectoryPoint command_pos = robot.convertReferenceFrame(current_pos, target_pos);

			command_pos.Position.CartesianPosition.X += current_pos.Coordinates.X;
			command_pos.Position.CartesianPosition.Y += current_pos.Coordinates.Y;
			command_pos.Position.CartesianPosition.Z += current_pos.Coordinates.Z;
			command_pos.Position.CartesianPosition.ThetaX += current_pos.Coordinates.ThetaX;
			command_pos.Position.CartesianPosition.ThetaY += current_pos.Coordinates.ThetaY;
			command_pos.Position.CartesianPosition.ThetaZ += current_pos.Coordinates.ThetaZ;

			for (int i=0; i<1; i++)
			{
				// send the command
				robot.sendScheduledTrajectory(command_pos);
				//usleep(1000);
			}

			op_state = finished;
			robot.sendRetractedTrajectory();
			return 0;
		}

	}
	
	return 0;
}



