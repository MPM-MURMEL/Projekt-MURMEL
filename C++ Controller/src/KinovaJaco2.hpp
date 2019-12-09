#ifndef _KINOVAJACO2_HPP_
#define _KINOVAJACO2_HPP_

#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include <KinovaTypes.h>
#include <Kinova.API.EthCommLayerUbuntu.h>
#include <Kinova.API.EthCommandLayerUbuntu.h>

#include <eigen3/Eigen/Dense>


/**
* A simple main function to demonstrate the usate of the Kinova class.
*
* @example TEST_KinovaJaco.cpp
* @example TEST_TCPClient_KinovaJaco.cpp
*/

/**
* @brief A wrapper class that provides easy access to the Kinova API
*
* The Kinova API provides a komplex interface to access and controle the Jaco 2. This class aims to simplify
* that interface as well as provide additional functionalities. 
*/
class KinovaJaco2
{

	private:

	/** 
	* @brief Kinova API Layer for API Communication
	*
	* Provides API feedback and information. 
	* Not directly responsible for the communication with the API, 
	* but rather an API pointer for initialization, and error handling.
	*/
	int apiLayer;

	/**
	* @brief Kinova API handler
	*
	* Contains the Kinova API functions by importing the corresponding shared object files.
	* All further functionalities are based on functions provided by this library import.
	*/
	void* commandLayer_handle;

	/**
	* @brief Trajectory Command scheduler of Jaco 2
	*
	* Can be used to get information about trajectory commands. 
	* The scheduler allows insight in current or futur trajectory events 
	* and can be used to plan or time trajectories.
	*/
	TrajectoryFIFO trajFifo;


	public:
	
	/**
	* @brief Kinova constructor
	*
	* Creates an robot istance to control. All necessary library call and initializations of the Kinova API are implemented here.
	*/
	KinovaJaco2();

	/**
	* @brief Kinova destructor
	*
	* Frees all Kinova related memory and libraries.
	*/
	~KinovaJaco2();

	/**
	* @brief Calibration routine for arm
	*
	* Moves the arm to a predefined home position for calibration purposes.
	* Provided by Kinova.
	*
	* @return Error code
	*/
	int(*initHome)();

	/**
	* @brief Calibration routine for fingers
	*
	* Moves the fingers to a predefined home position for calibration purposes.
	* Provided by Kinova.
	*
	* @return Error code
	*/
	int(*initFingers)();
	
	/**
	* @brief Refreshes device list
	*
	* Call to refresh the Kinova device list.
	* Provided by Kinova.
	*
	* @return Error code
	*/
	int(*refresDevicesList)();

	/**
	* @brief Get a list of devices
	*
	* Establishes an API connection and list currently connected devices.
	* The device list is not returned but rather a parameter reference.
	* Provided by Kinova.
	*
	* @param KinovaDevice[] Empty device list
	* @param result Value for apiLayer
	* @return Error code
	*/
	int(*getDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);

	/**
	* @brief Select a device
	*
	* Is used in combination with refreshDeviceList() and getDevices() in order to select a Kinova device if more then one is atteched.
	* Provided by Kinova.
	*
	* @param device Device to be used
	* @return Error code
	*/
	int(*setActiveDevice)(KinovaDevice device);

	/**
	* @brief Get type of control
	*
	* The response parameter is used to differentiate between cartesian and angular control mode.
	* Provided by Kinova.
	*
	* @param Response Return reference for control mode
	* @return Error code
	*/
	int(*getControlType)(int &Response);

	/**
	* @brief Set cartesian control mode
	*
	* Set type of control to cartesian mode. Only possible if the robot is NOT in an invalid position.
	* Provided by Kinova.
	*
	* @return Error code
	*/
	int(*setCartesianControl)();

	/**
	* @brief Set angular control mode
	*
	* Set type of control to angular mode. Might not be able to switch back to cartesian mode.
	* Provided by Kinova.
	*
	* @return Error code
	*/
	int(*setAngularControl)();

	/**
	* @brief Get cartesian position
	*
	* The CartesianPosition parameter is used to return the current position in cartesian coordinates.
	* The cartesian coordinates are the real sensor coordinates.
	* Provided by Kinova.
	*
	* @param pt Return reference for cartesian position
	* @return Error code
	*/
	int(*getCartesianPosition)(CartesianPosition& pt);

	/**
	* @brief Get cartesian position
	*
	* The CartesianPosition parameter is used to return the current position in cartesian coordinates.
	* The cartesian coordinates are the desired user coordinates previously send to the robot.
	* Provided by Kinova.
	*
	* @param pt Return reference for cartesian position
	* @return Error code
	*/
	int(*getCartesianCommand)(CartesianPosition& pt);

	/**
	* @brief Get angular position
	*
	* The AngularPosition parameter is used to return the current position in angular coordinates.
	* The angular coordinates are the real sensor coordinates.
	* Provided by Kinova.
	*
	* @param pt Return reference for angular position
	* @return Error code
	*/
	int(*getAngularPosition)(AngularPosition& pt);


	/**
	* @brief Get angular position
	*
	* The AngularPosition parameter is used to return the current position in angular coordinates.
	* The angular coordinates are the desired user coordinates previously send to the robot.
	* Provided by Kinova.
	*
	* @param pt Return reference for angular position
	* @return Error code
	*/
	int(*getAngularCommand)(AngularPosition& pt);


	/**
	* @brief Get trajectory information
	*
	* The TrajectoryFIFO parameter is used to return the trajectory scheduler information.
	* Can be used to time trajectory actions.
	* Provided by Kinova.
	*
	* @param Response Return reference for frajectory scheduler
	* @return Error code
	*/
	int(*getGlobalTrajectoryInfo)(TrajectoryFIFO &Response);

	/**
	* @brief Send simple trajectory
	*
	* Commands the robot to move to a certain position or with a certain velocity.
	* The basic trajectory does not allow trajectory constraints.
	* Provided by Kinova.
	*
	* @param trajectory Trajectory to move to
	* @return Error code
	*/
	int(*sendBasicTrajectory)(TrajectoryPoint trajectory);

	/**
	* @brief Send trajectory with constraints
	*
	* Commands the robot to move to a certain position or with a certain velocity.
	* Trajectory constraints can also be added.
	* Provided by Kinova.
	*
	* @param trajectory Trajectory to move to
	* @return Error code
	*/
	int(*sendAdvanceTrajectory)(TrajectoryPoint trajectory);

	/**
	* @brief Convert reference frame to base frame
	*
	* Converts movements in a reference frame to corresponding motions in the robot base frame.
	* Can be used to track object with a camera.
	*
	* @param pose Current position
	* @param cmdIn Trajectory in reference frame
	* @return Trajectory in base frame
	*/
	TrajectoryPoint convertReferenceFrame(CartesianPosition &pose, TrajectoryPoint &cmdIn);
	
	/**
	* @brief Move to home position
	*
	* Moves the robot to a user defined home position. 
	*/
	void sendHomeTrajectory();

	/**
	* @brief Move to retracted position
	*
	* Moves the robot to a user defined retracted (storage) position. 
	*/
	void sendRetractedTrajectory();

	/**
	* @brief Send and schedule trajectory
	*
	* Commands the robot to move to a certain position or with a certain velocity.
	* Waits for the robot to reach the requested position.
	*
	* @param trajectory Trajectory to move to
	*/
	void sendScheduledTrajectory(TrajectoryPoint trajectory);

	/**
	* @brief Sleep milliseconds
	*
	* @param ms Milliseconds to sleep
	*/
	void sleepMs(int ms);


	protected:

	/**
	* @brief Initializes API (USB)
	*
	* Initializes the Kinova USB API. USB is default for Kinova.
	*
	* @return API Layer information used for connection
	*/
	int(*initAPI)();

	/**
	* @brief Initializes API (Ethernet)
	*
	* Initializes the Kinova API for ethernet usage
	*
	* @return API Layer information used for connection
	*/
	int(*initEthernetAPI)(EthernetCommConfig & config);

	/**
	* @brief Close API
	*
	* Free API layer and related memory
	*
	* @return Error code
	*/
	int(*closeAPI)();
	
};

#endif

