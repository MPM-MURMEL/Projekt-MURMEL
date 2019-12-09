#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include "../src/KinovaJaco2.hpp"

#include <eigen3/Eigen/Dense>


using namespace std;


int main(int argc, char* argv[])
{
	// initialize Jaco2
	KinovaJaco2 robot = KinovaJaco2();

	// calibrate Jaco2
	robot.initHome();

	// send to retracted position
	robot.sendRetractedTrajectory();

	// send to home position
	robot.sendHomeTrajectory();


	// send robot x speed 100 time
	for (int p = 0; p<200; p++)
	{
		// trajectory command struct
		TrajectoryPoint commandIn;
		commandIn.InitStruct();
		commandIn.Position.HandMode = HAND_NOMOVEMENT; 
		commandIn.Position.Type = CARTESIAN_VELOCITY;

		// speed in every direction/orientation
		commandIn.Position.CartesianPosition.X = 0.1;// will go in x+ at 0.1 m/s
		commandIn.Position.CartesianPosition.Y = 0;
		commandIn.Position.CartesianPosition.Z = 0; 
		commandIn.Position.CartesianPosition.ThetaX = 0;
		commandIn.Position.CartesianPosition.ThetaY = 0;
		commandIn.Position.CartesianPosition.ThetaZ = 0;

		// get the current Cartesian position
		CartesianPosition CartPos;
		robot.getCartesianPosition(CartPos);

		// apply the right transformation to convert the speed command in the gripper reference frame
		TrajectoryPoint commandOut = robot.convertReferenceFrame(CartPos, commandIn);

		// send the command
		robot.sendScheduledTrajectory(commandOut);
	}

	// send robot y speed 100 time
	for (int p = 0; p<200; p++)
	{
		// trajectory command struct
		TrajectoryPoint commandIn;
		commandIn.InitStruct();
		commandIn.Position.HandMode = HAND_NOMOVEMENT; 
		commandIn.Position.Type = CARTESIAN_VELOCITY;

		// speed in every direction/orientation
		commandIn.Position.CartesianPosition.X = 0;
		commandIn.Position.CartesianPosition.Y = 0.1; // will go in y+ at 0.1 m/s
		commandIn.Position.CartesianPosition.Z = 0; 
		commandIn.Position.CartesianPosition.ThetaX = 0;
		commandIn.Position.CartesianPosition.ThetaY = 0;
		commandIn.Position.CartesianPosition.ThetaZ = 0;

		// get the current Cartesian position
		CartesianPosition CartPos;
		robot.getCartesianPosition(CartPos);

		// apply the right transformation to convert the speed command in the gripper reference frame
		TrajectoryPoint commandOut = robot.convertReferenceFrame(CartPos, commandIn);

		// send the command
		robot.sendScheduledTrajectory(commandOut);
	}

	// send robot z speed 100 time
	for (int p = 0; p<200; p++)
	{
		// trajectory command struct
		TrajectoryPoint commandIn;
		commandIn.InitStruct();
		commandIn.Position.HandMode = HAND_NOMOVEMENT; 
		commandIn.Position.Type = CARTESIAN_VELOCITY;

		// speed in every direction/orientation
		commandIn.Position.CartesianPosition.X = 0;
		commandIn.Position.CartesianPosition.Y = 0;
		commandIn.Position.CartesianPosition.Z = 0.1; // will go in z+ at 0.1 m/s
		commandIn.Position.CartesianPosition.ThetaX = 0;
		commandIn.Position.CartesianPosition.ThetaY = 0;
		commandIn.Position.CartesianPosition.ThetaZ = 0;

		// get the current Cartesian position
		CartesianPosition CartPos;
		robot.getCartesianPosition(CartPos);

		// apply the right transformation to convert the speed command in the gripper reference frame
		TrajectoryPoint commandOut = robot.convertReferenceFrame(CartPos, commandIn);

		// send the command
		robot.sendScheduledTrajectory(commandOut);
	}


	return 0;
}



