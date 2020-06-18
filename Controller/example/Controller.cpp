#include <iostream>
#include <fstream>
#include <sstream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>
#include <ctime>
#include <chrono>

#include <jsoncpp/json/json.h>
#include <eigen3/Eigen/Dense>

#include "../src/KinovaJaco2.hpp"
#include "../src/TCPClient.hpp"
#include "../src/PIDController.hpp"
#include "../src/ExponentialFilter.hpp"


using namespace std;


// dz distance to trash can thresh
const double dz_min = 0.04;

// offset for servoing
const double controller_offset_x = 0;
const double controller_offset_y = 0;
const double controller_offset_z = -0.08 - dz_min;
const double controller_offset_theta_x = 0;
const double controller_offset_theta_y = 0;

const double correction_offset_x = -0.013;
const double correction_offset_y = -0.09;
const double correction_offset_z = dz_min;

// x and y controll error threshold
const double x_y_thresh = 0.3;

// exponential filter factor
const double exp_w = 0.2;

// gain of PID-Controller
const double pid_p = 4;

// integral value of PID-Controller
const double pid_i = 0;

// differential value of PID-Controller
const double pid_d = 0;


int openTrashCanDemo(KinovaJaco2 &robot)
{

	//---------------------------------------------	
	// APPROACH
	//---------------------------------------------	

	// start tcp client
	cout << "[INFO] Starting TCP Client..." << endl;
	TCPClient client("192.168.100.20", 8080); 
	
	if (client.isConnected())
	{
		cout << "[INFO] Connected" << endl;
		
		ExponentialFilter f_prob = ExponentialFilter(exp_w);			

		ExponentialFilter f_x = ExponentialFilter(exp_w);
		ExponentialFilter f_y = ExponentialFilter(exp_w);
		ExponentialFilter f_r = ExponentialFilter(exp_w);
		ExponentialFilter f_theta_x = ExponentialFilter(exp_w);			
		ExponentialFilter f_theta_y = ExponentialFilter(exp_w);			


		// create pid controllers for x and y direction
		PIDController p_x = PIDController(pid_p, pid_i, pid_d);
		PIDController p_y = PIDController(pid_p, pid_i, pid_d);
		PIDController p_z = PIDController(pid_p, pid_i, pid_d);
		PIDController p_theta_x = PIDController(0.01, pid_i, pid_d);
		PIDController p_theta_y = PIDController(0.01, pid_i, pid_d);

		// send tracking command to camera
		client.send("{\"MODE\":\"TRACKING\"}");


		// timestamp for csv file output
		time_t file_time = time(0);
		string file_name(ctime(&file_time));
		file_name.append(".csv");

		ofstream out_file;
		out_file.open(file_name);

		// csv header for all necessary parameters
		out_file << "file name:\t" << file_name << endl;

		out_file << "filter value probability:\t" << exp_w << endl;
		out_file << "filter value x:\t" << exp_w << endl;
		out_file << "filter value y:\t" << exp_w << endl;
		out_file << "filter value z:\t" << exp_w << endl;
		out_file << "filter value theta_x:\t" << exp_w << endl;
		out_file << "filter value theta_y:\t" <<exp_w << endl;

		out_file << "controller value x:\t" << pid_p << "\t" << pid_i << "\t" << pid_d << endl;
		out_file << "controller value y:\t" << pid_p << "\t" << pid_i << "\t" << pid_d << endl;
		out_file << "controller value z:\t" << pid_p << "\t" << pid_i << "\t" << pid_d << endl;
		out_file << "controller value theta_x:\t" << pid_p << "\t" << pid_i << "\t" << pid_d << endl;
		out_file << "controller value theta_y:\t" << pid_p << "\t" << pid_i << "\t" << pid_d << endl;

		out_file << "camera MODE:\t" << "TRACKING" << endl;
	
		// variables vor csv data set
		out_file << "iteration\t" << "time\t" << "raw_probability\t" << "raw_x\t" << "raw_y\t" << "raw_z\t" << "raw_theta_x\t" << "raw_theta_y\t";
		out_file << "filter_probability\t" << "filter_x\t" << "filter_y\t" << "filter_z\t" << "filter_theta_x\t" << "filter_theta_y\t";
		out_file << "offset_x\t" << "offset_y\t" << "offset_z\t" << "offset_theta_x\t" << "offset_theta_y\t";
		out_file << "control_x\t" << "control_y\t" << "control_z\t" << "control_theta_x\t" << "control_theta_y\t";
		out_file << "transform_x\t" << "transform_y\t" << "transform_z\t" << "transform_theta_x\t" << "transform_theta_y" << endl;

		// start time of control loop 
		chrono::milliseconds start_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()); 

		// counter for skip of first messages
		int counter = 0;

		// check if client is connected to server
		while(client.isConnected())
		{
			double probability = 0;
			double dx = 0;
			double dy = 0;
			double dz = 0;

			double theta_x = 0;
			double theta_y = 0;


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
					probability = root["probability"].asDouble();

					// cast cand convert from mm to meter
					dx = root["dx"].asDouble() / 1000;
					dy = root["dy"].asDouble() / 1000;

					// needs to be inverted 
					// zero starts at trash can surface not camera lense
					dz = -1 * root["dz"].asDouble() / 1000;

					theta_x = root["theta_x"].asDouble();
					theta_y = -1* root["theta_y"].asDouble();
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
			

			// output string object for plotting
			stringstream ss;

			// control loop iteration time
			chrono::milliseconds run_ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()); 

			//save raw camera data to stream
			ss << counter << "\t" << (run_ms.count()-start_ms.count()) << "\t" << probability << "\t" << dx << "\t" << dy << "\t" << dz << "\t" << theta_x << "\t" << theta_y << "\t";



			//only controll if probability high enough and first few dz skipped
			if (10 < counter++ && probability > 0)
			{

				// calculate filtered values
				probability = f_prob.calculate(probability);
				dx = f_x.calculate(dx);
				dy = f_y.calculate(dy);
				dz = f_r.calculate(dz);
				theta_x = f_theta_x.calculate(theta_x);
				theta_y = f_theta_y.calculate(theta_y);

				

				// save filtervalues to stream
				ss << probability << "\t" << dx << "\t" << dy << "\t" << dz << "\t" << theta_x << "\t" << theta_y << "\t";
			

				// calculate controll values
				// speed needs to be influenced by distance
				dx = p_x.calculate(controller_offset_x - dx);
				dy = p_y.calculate(controller_offset_y - dy);
				dz = p_y.calculate(controller_offset_z - dz);
				theta_x = p_theta_x.calculate(controller_offset_theta_x - theta_x);
				theta_y = p_theta_y.calculate(controller_offset_theta_y - theta_y);

				// save offsets to stream
				ss << controller_offset_x << "\t" << controller_offset_y << "\t" << controller_offset_z << "\t" << controller_offset_theta_x << "\t" << controller_offset_theta_y << "\t";

				//save control value to stream
				ss << dx << "\t" << dy << "\t" << dz << "\t" << theta_x << "\t" << theta_y << "\t";

				cout << "dx: " << dx << endl;
				cout << "dy: " << dy << endl;
				cout << "dz: " << dz << endl;
				cout << "dtheta_x: " << theta_x << endl;
				cout << "dtheta_y: " << theta_y << endl <<endl;



				// change op_state
				if (dz <= 0 && client.isConnected())
				{	
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

				cout << "dz: " << dz << endl << endl;

				// trajectory command struct
				TrajectoryPoint target_pos;
				target_pos.InitStruct();
				target_pos.Position.HandMode = HAND_NOMOVEMENT; 
				target_pos.Position.Type = CARTESIAN_VELOCITY;

				// speed in every direction/orientation
				target_pos.Position.CartesianPosition.X = dx;
				target_pos.Position.CartesianPosition.Y = dy;
				target_pos.Position.CartesianPosition.Z = dz; 
				target_pos.Position.CartesianPosition.ThetaX = theta_x;
				target_pos.Position.CartesianPosition.ThetaY = theta_y;
				target_pos.Position.CartesianPosition.ThetaZ = 0;

				//get the current Cartesian position
				CartesianPosition current_pos;
				robot.getCartesianPosition(current_pos);

				// apply the right transformation to convert the speed command in the gripper reference frame
				// orbbec and refframe are misoriented by 90deg around z
				TrajectoryPoint command_pos = robot.convertReferenceFrame(current_pos, target_pos);


				// send the command
				robot.sendScheduledTrajectory(command_pos);


				ss << command_pos.Position.CartesianPosition.X << "\t" << command_pos.Position.CartesianPosition.Y << "\t" << command_pos.Position.CartesianPosition.Z << "\t" ;
				ss << command_pos.Position.CartesianPosition.ThetaX << "\t" << command_pos.Position.CartesianPosition.ThetaY << endl;

				out_file << ss.str();
				
			}
		

		}

		cout << "[INFO] Connection lost" << endl;

		out_file.close();

	}
	else
	{
		return 1;
	}



	//---------------------------------------------	
	// INSERT
	//---------------------------------------------	

	cout << "[INFO] Insert Key" << endl;

	// trajectory command struct
	TrajectoryPoint target_pos;

	target_pos.InitStruct();
	target_pos.Position.HandMode = HAND_NOMOVEMENT; 
	target_pos.Position.Type = CARTESIAN_POSITION;

	// speed in every direction/orientation
	target_pos.Position.CartesianPosition.X = correction_offset_x;
	target_pos.Position.CartesianPosition.Y = correction_offset_y;
	target_pos.Position.CartesianPosition.Z = 0;
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

	command_pos.Position.CartesianPosition.ThetaX = current_pos.Coordinates.ThetaX;
	command_pos.Position.CartesianPosition.ThetaY = current_pos.Coordinates.ThetaY;
	command_pos.Position.CartesianPosition.ThetaZ = current_pos.Coordinates.ThetaZ;


	while (!robot.isAtTarget(command_pos))
	{
		// send the command
		robot.sendScheduledTrajectory(command_pos);
	}


	// trajectory command struct
	target_pos.InitStruct();
	target_pos.Position.HandMode = HAND_NOMOVEMENT; 
	target_pos.Position.Type = CARTESIAN_POSITION;

	// speed in every direction/orientation
	target_pos.Position.CartesianPosition.X = 0;
	target_pos.Position.CartesianPosition.Y = 0;
	target_pos.Position.CartesianPosition.Z = correction_offset_z; 
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

	command_pos.Position.CartesianPosition.ThetaX = current_pos.Coordinates.ThetaX;
	command_pos.Position.CartesianPosition.ThetaY = current_pos.Coordinates.ThetaY;
	command_pos.Position.CartesianPosition.ThetaZ = current_pos.Coordinates.ThetaZ;

	while (!robot.isAtTarget(command_pos))
	{
		// send the command
		robot.sendScheduledTrajectory(command_pos);
	}


	//---------------------------------------------	
	// OPEN
	//---------------------------------------------	

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

	while (!robot.isAtTarget(command_angles))
	{
		// send the command
		robot.sendScheduledTrajectory(command_angles);
	}



	//---------------------------------------------	
	// EXTRACT
	//---------------------------------------------	

	cout << "[INFO] Extracting key from keyhole" << endl;

	// get current angular position from robot
	robot.getAngularCommand(current_angles);


	// definition of retracted position in angular mode
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

	while (!robot.isAtTarget(command_angles))
	{
		// send the command
		robot.sendScheduledTrajectory(command_angles);
	}

	// trajectory command struct
	target_pos.InitStruct();
	target_pos.Position.HandMode = HAND_NOMOVEMENT; 
	target_pos.Position.Type = CARTESIAN_POSITION;

	// speed in every direction/orientation
	target_pos.Position.CartesianPosition.X = 0;
	target_pos.Position.CartesianPosition.Y = 0;
	target_pos.Position.CartesianPosition.Z = -correction_offset_z; 
	target_pos.Position.CartesianPosition.ThetaX = 0;
	target_pos.Position.CartesianPosition.ThetaY = 0;
	target_pos.Position.CartesianPosition.ThetaZ = 0;

	//get the current Cartesian position
	robot.getCartesianCommand(current_pos);

	// apply the right transformation to convert the speed command in the gripper reference frame
	// orbbec and refframe are misoriented by 90deg around z
	command_pos = robot.convertReferenceFrame(current_pos, target_pos);

	command_pos.Position.CartesianPosition.X += current_pos.Coordinates.X;
	command_pos.Position.CartesianPosition.Y += current_pos.Coordinates.Y;
	command_pos.Position.CartesianPosition.Z += current_pos.Coordinates.Z;

	command_pos.Position.CartesianPosition.ThetaX = current_pos.Coordinates.ThetaX;
	command_pos.Position.CartesianPosition.ThetaY = current_pos.Coordinates.ThetaY;
	command_pos.Position.CartesianPosition.ThetaZ = current_pos.Coordinates.ThetaZ;

	while (!robot.isAtTarget(command_pos))
	{
		// send the command
		robot.sendScheduledTrajectory(command_pos);
	}

	return 0;
}


/**
* @brief Starts the robot controller software
*
* @param argc number of arguments
* @param argv list of arguments
*
* if "./Controller" demonstration example für trash can opening
* if "./Controller home" the homeing function is called and the robot will
* if "./Controller retract" the robot will fold into its retracted position
* if "./Controller calibrate" robot will calibrate ists joints. sometime necessary to fix movement issues.
*/
int main(int argc, char* argv[])
{	
	if (argc > 1)
	{
		// initialize Jaco2
		cout << "[INFO] Starting KinovaJaco2..." << endl;
		KinovaJaco2 robot = KinovaJaco2();

		// option string from argv list
		string arg_option(argv[1]);

		if (arg_option.find("calibrate")!=string::npos)
		{
			// calibrate Jaco2
			cout << "[INFO] Calibrate robot..." << endl;
			robot.initHome();
			return 0;
		}	

		if (arg_option.find("retract")!=string::npos)
		{
			// send to retracted position
			cout << "[INFO] Retract robot..." << endl;
			robot.sendRetractedTrajectory();
			return 0;
		}	

		if (arg_option.find("home")!=string::npos)
		{
			// send to home position
			cout << "[INFO] Homing robot..." << endl;
			robot.sendHomeTrajectory();
			return 0;
		}	

		if (arg_option.find("demo")!=string::npos)
		{
			// send to home position
			cout << "[INFO] Homing robot..." << endl;
			robot.sendHomeTrajectory();

			// open keyhole demo
			cout << "[INFO] Keyhole demo..." << endl;
			openTrashCanDemo(robot);

			// send to retracted position
			cout << "[INFO] Retract robot..." << endl;
			robot.sendRetractedTrajectory();
			return 0;
		}	

		cout << "Please use a mode parameter.\n" << endl;
		cout << "- ./Controller calibrate : calibrates the roboter joints home position." << endl;
		cout << "- ./Controller retract : retracts the robot to a compact holding position." << endl;
		cout << "- ./Controller home : robot moves to workposition." << endl;
		cout << "- ./Controller demo : robot moves to workposition, finds and opens\ntrashcan, then retracts to the holding position.\n" << endl;

	}
	else
	{
		cout << "Please use a mode parameter.\n" << endl;
		cout << "- ./Controller calibrate : calibrates the roboter joints home position." << endl;
		cout << "- ./Controller retract : retracts the robot to a compact holding position." << endl;
		cout << "- ./Controller home : robot moves to workposition." << endl;
		cout << "- ./Controller demo : robot moves to workposition, finds and opens\ntrashcan, then retracts to the holding position.\n" << endl;
	}
}

