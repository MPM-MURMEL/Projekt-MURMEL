#include "KinovaJaco2.hpp"



KinovaJaco2::KinovaJaco2()
{
	if (loadAPI("../lib/Kinova.API.EthCommandLayerUbuntu.so"))
	{
		apiLayer = initAPI();

		if(apiLayer > 2000)
		{
			wcout << "Error: API not initialized (" << apiLayer << ")" << endl;
			exit(1);
		}

		initRobot(0);
	}
	else
	{
		wcout << "Error: API not found" << endl;
		exit(1);
	}
}


KinovaJaco2::KinovaJaco2(EthernetCommConfig config)
{
	if (loadAPI("../lib/Kinova.API.EthCommandLayerUbuntu.so"))
	{
		apiLayer = initEthernetAPI(config);
		
		if(apiLayer)
		{
			wcout << "Error: API not initialized (" << apiLayer << ")" << endl;
			exit(1);
		}

		initRobot(0);
	}
	else
	{
		wcout << "Error: API not found" << endl;
		exit(1);
	}
}


KinovaJaco2::~KinovaJaco2()
{
	if(apiLayer > 0 && apiLayer < 2000)
	{
		apiLayer = closeAPI();
	}

	dlclose(commandLayer_handle);
}


int KinovaJaco2::loadAPI(string path)
{
	//load the library
    	//commandLayer_handle = dlopen("../lib/Kinova.API.EthCommandLayerUbuntu.so",RTLD_NOW|RTLD_GLOBAL);
	commandLayer_handle = dlopen(path.c_str(), RTLD_NOW|RTLD_GLOBAL);

	//load the functions from the library
	initAPI = (int (*)()) dlsym(commandLayer_handle,"Ethernet_InitAPI");
	initEthernetAPI = (int (*)(EthernetCommConfig & config)) dlsym(commandLayer_handle,"Ethernet_InitEthernetAPI");
	closeAPI = (int (*)()) dlsym(commandLayer_handle,"Ethernet_CloseAPI");

	refresDevicesList = (int (*)()) dlsym(commandLayer_handle,"Ethernet_RefresDevicesList");
	getDevices = (int (*)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result)) dlsym(commandLayer_handle,"Ethernet_GetDevices");
	setActiveDevice = (int (*)(KinovaDevice devices)) dlsym(commandLayer_handle,"Ethernet_SetActiveDevice");

	initHome = (int (*)()) dlsym(commandLayer_handle,"Ethernet_MoveHome");
	initFingers = (int (*)()) dlsym(commandLayer_handle,"Ethernet_InitFingers");

	setCartesianControl = (int (*)()) dlsym(commandLayer_handle,"Ethernet_SetCartesianControl");
	setAngularControl = (int (*)()) dlsym(commandLayer_handle,"Ethernet_SetAngularControl");
	getControlType = (int (*)(int &Response)) dlsym(commandLayer_handle,"Ethernet_GetControlType");

	getCartesianPosition = (int (*)(CartesianPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetCartesianPosition");
	getAngularPosition = (int (*)(AngularPosition& pt)) dlsym(commandLayer_handle,"Ethernet_GetAngularPosition");
	getGlobalTrajectoryInfo = (int (*)(TrajectoryFIFO &Response)) dlsym(commandLayer_handle,"Ethernet_GetGlobalTrajectoryInfo");

	sendBasicTrajectory = (int (*)(TrajectoryPoint trajectory)) dlsym(commandLayer_handle,"Ethernet_SendBasicTrajectory");
	sendAdvanceTrajectory = (int (*)(TrajectoryPoint trajectory)) dlsym(commandLayer_handle,"Ethernet_SendAdvanceTrajectory");


	//If the was loaded correctly
	if ((initAPI == NULL) || (closeAPI == NULL) || (getDevices == NULL) || (setCartesianControl == NULL) || (setAngularControl == NULL) 
		|| (initHome == NULL) || (initFingers == NULL) || (sendBasicTrajectory == NULL) || (sendAdvanceTrajectory == NULL) 
		|| (getCartesianPosition == NULL) || (setActiveDevice == NULL) || (getAngularPosition == NULL) || (initEthernetAPI == NULL) 
		|| (getGlobalTrajectoryInfo == NULL) || (getControlType == NULL))
	{
		//return false
		return 0;
	}

	return 1;
}


int KinovaJaco2::initRobot(int id)
{
	refresDevicesList();
	KinovaDevice list[MAX_KINOVA_DEVICE];
	getDevices(list, apiLayer);

	if (sizeof(list)/sizeof(KinovaDevice) > 0)
	{
		setActiveDevice(list[id]);

		initHome();
		//initFingers();
		setCartesianControl();

		return 1;
	}

	return 0;
}


int KinovaJaco2::sendTrajectory(TrajectoryPoint trajectory)
{
	int result = sendAdvanceTrajectory(trajectory);

	do
	{
		getGlobalTrajectoryInfo(trajFifo);
		sleepMs(1);
	} 
	while(trajFifo.TrajectoryCount >=1);
	sleepMs(5);

	return result;
}


int KinovaJaco2::sendHomeTrajectory()
{
	TrajectoryPoint cmd;
	cmd.InitStruct();
	cmd.Position.HandMode = HAND_NOMOVEMENT; 
	cmd.Position.Type = ANGULAR_POSITION;

	cmd.Position.Actuators.Actuator1 = 275;
	cmd.Position.Actuators.Actuator2 = 200;
	cmd.Position.Actuators.Actuator3 = 25;
	cmd.Position.Actuators.Actuator4 = 180;
	cmd.Position.Actuators.Actuator5 = 268;
	cmd.Position.Actuators.Actuator6 = 275;

	cmd.Position.Fingers.Finger1 = 0;
	cmd.Position.Fingers.Finger2 = 0;
	cmd.Position.Fingers.Finger3 = 0;

		
	return sendTrajectory(cmd);
}


int KinovaJaco2::sendRetractedTrajectory()
{
	TrajectoryPoint cmd;
	cmd.InitStruct();
	cmd.Position.HandMode = HAND_NOMOVEMENT; 
	cmd.Position.Type = ANGULAR_POSITION;

	cmd.Position.Actuators.Actuator1 = 360;
	cmd.Position.Actuators.Actuator2 = 135;
	cmd.Position.Actuators.Actuator3 = 20;
	cmd.Position.Actuators.Actuator4 = 180;
	cmd.Position.Actuators.Actuator5 = 150;
	cmd.Position.Actuators.Actuator6 = 270;
	cmd.Position.Fingers.Finger1 = 0;
	cmd.Position.Fingers.Finger2 = 0;
	cmd.Position.Fingers.Finger3 = 0;

	return sendTrajectory(cmd);
}


void KinovaJaco2::sleepMs(int ms)
{
    usleep(ms * 1000);
}



