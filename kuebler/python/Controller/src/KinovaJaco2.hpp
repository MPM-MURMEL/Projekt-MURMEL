#include <iostream>
#include <dlfcn.h>
#include <vector>
#include <stdio.h>
#include <unistd.h>

#include <KinovaTypes.h>
#include <Kinova.API.EthCommLayerUbuntu.h>
#include <Kinova.API.EthCommandLayerUbuntu.h>

using namespace std;


class KinovaJaco2
{

// VARIABLES

public:

protected:

private:
	// used to communicate with Kinova api shared object files
	void* commandLayer_handle;

	// return value of api initialization
	int apiLayer;

	// trajectory command sceduler
	TrajectoryFIFO trajFifo;



//FUNCTIONS

public:
	KinovaJaco2();
	KinovaJaco2(EthernetCommConfig config);
	~KinovaJaco2();

	//device management
	int(*refresDevicesList)();
	int(*setActiveDevice)(KinovaDevice device);
	int(*getDevices)(KinovaDevice devices[MAX_KINOVA_DEVICE], int &result);

	// init
	int(*initHome)();
	int(*initFingers)();

	// movement
	int(*setCartesianControl)();
	int(*setAngularControl)();

	int(*getControlType)(int &response);

	int(*getCartesianPosition)(CartesianPosition& position);
	int(*getAngularPosition)(AngularPosition& position);
	int(*getGlobalTrajectoryInfo)(TrajectoryFIFO &response);

	int(*sendBasicTrajectory)(TrajectoryPoint trajectory);
	int(*sendAdvanceTrajectory)(TrajectoryPoint trajectory);
	
	int sendTrajectory(TrajectoryPoint trajectory);
	int sendHomeTrajectory();
	int sendRetractedTrajectory();

	void sleepMs(int ms);

protected:
	int(*initAPI)();
	int(*initEthernetAPI)(EthernetCommConfig & config);
	int(*closeAPI)();

	int loadAPI(string path);
	int initRobot(int id);

private:
	


};

