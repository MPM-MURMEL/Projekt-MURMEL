#include "KinovaJaco2.hpp"


KinovaJaco2::KinovaJaco2()
{
	//load the library
    commandLayer_handle = dlopen("lib/Kinova.API.EthCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	
	//load the functions from the library
	initAPI = (int (*)()) dlsym(commandLayer_handle,"Ethernet_InitAPI");
	closeAPI = (int (*)()) dlsym(commandLayer_handle,"Ethernet_CloseAPI");
	initEthernetAPI = (int (*)(EthernetCommConfig & config)) dlsym(commandLayer_handle,"Ethernet_InitEthernetAPI");

	initHome = (int (*)()) dlsym(commandLayer_handle,"Ethernet_MoveHome");
	initFingers = (int (*)()) dlsym(commandLayer_handle,"Ethernet_InitFingers");

	refresDevicesList = (int (*)()) dlsym(commandLayer_handle,"Ethernet_RefresDevicesList");
	getDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,"Ethernet_GetDevices");
	setActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle,"Ethernet_SetActiveDevice");

	getControlType = (int (*)(int &Response)) dlsym(commandLayer_handle,"Ethernet_GetControlType");
	setCartesianControl = (int (*)()) dlsym(commandLayer_handle,"Ethernet_SetCartesianControl");
	setAngularControl = (int (*)()) dlsym(commandLayer_handle,"Ethernet_SetAngularControl");

	getGlobalTrajectoryInfo = (int (*)(TrajectoryFIFO &Response)) dlsym(commandLayer_handle,"Ethernet_GetGlobalTrajectoryInfo");
	getCartesianPosition = (int (*)(CartesianPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetCartesianPosition");
	getCartesianCommand = (int (*)(CartesianPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetCartesianCommand");
	getAngularPosition = (int (*)(AngularPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetAngularPosition");
	getAngularCommand = (int (*)(AngularPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetAngularCommand");

	sendBasicTrajectory = (int (*)(TrajectoryPoint trajectory)) dlsym(commandLayer_handle,"Ethernet_SendBasicTrajectory");
	sendAdvanceTrajectory = (int (*)(TrajectoryPoint trajectory)) dlsym(commandLayer_handle,"Ethernet_SendAdvanceTrajectory");



	//If the was loaded correctly
	if ((initAPI == NULL) || (closeAPI == NULL) || (getDevices == NULL) || (setCartesianControl == NULL) || (setAngularControl == NULL) || (initHome == NULL)
		|| (initFingers == NULL) || (sendBasicTrajectory == NULL) || (sendAdvanceTrajectory == NULL) || (getCartesianPosition == NULL)
		|| (setActiveDevice == NULL) || (getAngularPosition == NULL) || (initEthernetAPI == NULL) || (getGlobalTrajectoryInfo == NULL) || (getControlType == NULL))
	{
		std::cout << "[ERROR] Kinova command layer not initilaized" << std::endl;	
		exit(1);
	}
	else
	{
		std::cout << "[INFO] Kinova initialization complete" << std::endl;	
		
		//init api layer
		apiLayer = initAPI();

		// find and set active robot device
		refresDevicesList();
		KinovaDevice list[MAX_KINOVA_DEVICE];
		int devicesCount = getDevices(list, apiLayer);
		setActiveDevice(list[0]);
	}
}


KinovaJaco2::~KinovaJaco2()
{
	//free api layer and libraries if they were used
	if(apiLayer)
	{
		apiLayer = closeAPI();
	}

	dlclose(commandLayer_handle);
}


TrajectoryPoint KinovaJaco2::convertReferenceFrame(CartesianPosition &pose, TrajectoryPoint &cmdIn)
{
	TrajectoryPoint cmdOut;

	cmdOut = cmdIn;

	//the transformation matrix between end-effector and base frame is given by the end-effector orientation
	Eigen::Matrix3f orientation_robot;
	orientation_robot = Eigen::AngleAxisf(pose.Coordinates.ThetaX, Eigen::Vector3f::UnitX())
						* Eigen::AngleAxisf(pose.Coordinates.ThetaY, Eigen::Vector3f::UnitY())
						* Eigen::AngleAxisf(pose.Coordinates.ThetaZ, Eigen::Vector3f::UnitZ());

	//translation vector in end-effector frame
	Eigen::Vector3f translation_vector_ee_frame(cmdIn.Position.CartesianPosition.X, cmdIn.Position.CartesianPosition.Y, cmdIn.Position.CartesianPosition.Z);

	//translation vector in base frame
	Eigen::Vector3f translation_vector_base_frame = orientation_robot * translation_vector_ee_frame;

	//update the x-y-z part of the command
	cmdOut.Position.CartesianPosition.X = translation_vector_base_frame(0); // X
	cmdOut.Position.CartesianPosition.Y = translation_vector_base_frame(1); // Y
	cmdOut.Position.CartesianPosition.Z = translation_vector_base_frame(2); // Z

	return cmdOut;
}


void KinovaJaco2::sendHomeTrajectory()
{
	// definition of home position in angular mode
	TrajectoryPoint cmd;
	cmd.InitStruct();
	cmd.Position.Type = ANGULAR_POSITION;

	cmd.Position.Actuators.Actuator1 = 275;
	cmd.Position.Actuators.Actuator2 = 200;
	cmd.Position.Actuators.Actuator3 = 25;
	cmd.Position.Actuators.Actuator4 = 180;
	cmd.Position.Actuators.Actuator5 = 268; // 300
	cmd.Position.Actuators.Actuator6 = 180;

	cmd.Position.Fingers.Finger1 = 0;
	cmd.Position.Fingers.Finger2 = 0;
	cmd.Position.Fingers.Finger3 = 0;

	while (!isAtTarget(cmd))
	{
		// send home position	
		sendScheduledTrajectory(cmd);
	}

}


void KinovaJaco2::sendRetractedTrajectory()
{
	// definition of retracted position in angular mode
	TrajectoryPoint cmd;
	cmd.InitStruct();
	cmd.Position.Type = ANGULAR_POSITION;

	cmd.Position.Actuators.Actuator1 = 360;
	cmd.Position.Actuators.Actuator2 = 135;
	cmd.Position.Actuators.Actuator3 = 20;
	cmd.Position.Actuators.Actuator4 = 180;
	cmd.Position.Actuators.Actuator5 = 150;
	cmd.Position.Actuators.Actuator6 = 180;
	cmd.Position.Fingers.Finger1 = 0;
	cmd.Position.Fingers.Finger2 = 0;
	cmd.Position.Fingers.Finger3 = 0;

	while (!isAtTarget(cmd))
	{
		// send home position
		sendScheduledTrajectory(cmd);
	}
}


void KinovaJaco2::sendScheduledTrajectory(TrajectoryPoint trajectory)
{
	// use api function for trajectory
	sendAdvanceTrajectory(trajectory);

	//wait until trajectory was processed
	do
	{
		getGlobalTrajectoryInfo(trajFifo);
		sleepMs(1);
	} 
	while(trajFifo.TrajectoryCount >=1);
	sleepMs(5);
}


bool KinovaJaco2::isAtTarget(TrajectoryPoint target_trajectory)
{
	if (target_trajectory.Position.Type == ANGULAR_POSITION)
	{
		//get the current Cartesian position
		AngularPosition current_angles;
		getAngularCommand(current_angles);

		// add all errors of translation
		double error = abs(current_angles.Actuators.Actuator1 - target_trajectory.Position.Actuators.Actuator1)
		+ abs(current_angles.Actuators.Actuator2 - target_trajectory.Position.Actuators.Actuator2)
		+ abs(current_angles.Actuators.Actuator3 - target_trajectory.Position.Actuators.Actuator3)
		+ abs(current_angles.Actuators.Actuator4 - target_trajectory.Position.Actuators.Actuator4)
		+ abs(current_angles.Actuators.Actuator5 - target_trajectory.Position.Actuators.Actuator5)
		+ abs(current_angles.Actuators.Actuator6 - target_trajectory.Position.Actuators.Actuator6);

		return error < 0.1;
	}
	else if (target_trajectory.Position.Type == CARTESIAN_POSITION)
	{
		//get the current Cartesian position
		CartesianPosition current_pos;
		getCartesianCommand(current_pos);

		// add all errors of translation
		double error_trans = abs(current_pos.Coordinates.X - target_trajectory.Position.CartesianPosition.X)
		+ abs(current_pos.Coordinates.Y - target_trajectory.Position.CartesianPosition.Y)
		+ abs(current_pos.Coordinates.Z - target_trajectory.Position.CartesianPosition.Z);

		//add all errors of rotation
	 	double error_angle = abs(current_pos.Coordinates.ThetaX - target_trajectory.Position.CartesianPosition.ThetaX)
		+ abs(current_pos.Coordinates.ThetaY - target_trajectory.Position.CartesianPosition.ThetaY)
		+ abs(current_pos.Coordinates.ThetaZ - target_trajectory.Position.CartesianPosition.ThetaZ);

		return error_trans < 0.01 && error_angle < 1;
	}

	return false;
}


void KinovaJaco2::sleepMs(int ms)
{
	// convert millis to micro
    	usleep(ms * 1000);
}

