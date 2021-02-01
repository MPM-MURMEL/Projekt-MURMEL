# Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`class `[`ExponentialFilter`](#classExponentialFilter) | A simple exponential Filter class.
`class `[`KinovaJaco2`](#classKinovaJaco2) | A wrapper class that provides easy access to the Kinova API.
`class `[`PIDController`](#classPIDController) | A simple PID controller class.
`class `[`TCPClient`](#classTCPClient) | A simple TCP client class.

# class `ExponentialFilter` 

A simple exponential Filter class.

A low pass filter based on the recusive exponential filter method

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`ExponentialFilter`](#classExponentialFilter_1a94dc04e65a5134545b5a1541adf63c4c)`(double w)` | Exponential filter constructor.
`public  `[`~ExponentialFilter`](#classExponentialFilter_1a68e52952db9dd3f2a42fd2fac7f45ed0)`()` | Exponential filter constructor destructor.
`public void `[`setWeight`](#classExponentialFilter_1a578bf205cc781e1a003998a35c610a94)`(double w)` | Set weight of exponential filter.
`public double `[`calculate`](#classExponentialFilter_1a531ad3c526275977e40cc241f6943a8c)`(double s_in)` | Calculates new smooth signal value.

## Members

#### `public  `[`ExponentialFilter`](#classExponentialFilter_1a94dc04e65a5134545b5a1541adf63c4c)`(double w)` 

Exponential filter constructor.

Creates an exponential filter. This filter can be used to smooth incoming signals based on previous signal values.

#### Parameters
* `w` weight that controlls the amount of smoothing

#### `public  `[`~ExponentialFilter`](#classExponentialFilter_1a68e52952db9dd3f2a42fd2fac7f45ed0)`()` 

Exponential filter constructor destructor.

Frees all exponential filter related memory.

#### `public void `[`setWeight`](#classExponentialFilter_1a578bf205cc781e1a003998a35c610a94)`(double w)` 

Set weight of exponential filter.

Sets the weight used to calculate smooth values from incoming signal values. 
#### Parameters
* `w` weight that controlls the amount of smoothing

#### `public double `[`calculate`](#classExponentialFilter_1a531ad3c526275977e40cc241f6943a8c)`(double s_in)` 

Calculates new smooth signal value.

Calculates the outgoing smooth signal value based on the previously calculated value. s_out = w * s_in + (1 - w) * s_out_old

#### Parameters
* `s_in` input signal 

#### Returns
smooth output signal (s_out)

# class `KinovaJaco2` 

A wrapper class that provides easy access to the Kinova API.

The Kinova API provides a komplex interface to access and controle the Jaco 2. This class aims to simplify that interface as well as provide additional functionalities.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public int(* `[`initHome`](#classKinovaJaco2_1a425ca9acefe1571e4ced89b859b50fb1) | Calibration routine for arm.
`public int(* `[`initFingers`](#classKinovaJaco2_1a9c1405d6669a50f41f52a9820035f216) | Calibration routine for fingers.
`public int(* `[`refresDevicesList`](#classKinovaJaco2_1a87f83b83ae1d298ed6fc56c926b04826) | Refreshes device list.
`public int(* `[`getDevices`](#classKinovaJaco2_1a764a8497c6e804dc357926243329d3ab) | Get a list of devices.
`public int(* `[`setActiveDevice`](#classKinovaJaco2_1a4f1954df31d33fb5bd8bfee3344b5d48) | Select a device.
`public int(* `[`getControlType`](#classKinovaJaco2_1a4226acf96370a26f3eba2d31461ca627) | Get type of control.
`public int(* `[`setCartesianControl`](#classKinovaJaco2_1aecef09554385d1fe4f48a875ffd0e018) | Set cartesian control mode.
`public int(* `[`setAngularControl`](#classKinovaJaco2_1a84a9ea5b49dbe3ef33aa33a31b41df54) | Set angular control mode.
`public int(* `[`getCartesianPosition`](#classKinovaJaco2_1afab6fbdde7d95c6e3667bd0b89b6ad78) | Get cartesian position.
`public int(* `[`getCartesianCommand`](#classKinovaJaco2_1a7dd5153bdfa220e290c96910eea89764) | Get cartesian position.
`public int(* `[`getAngularPosition`](#classKinovaJaco2_1aa98af0706e3a466d8ab9036c6cc7d0b3) | Get angular position.
`public int(* `[`getAngularCommand`](#classKinovaJaco2_1aba62e119d7720205c34356815610e83a) | Get angular position.
`public int(* `[`getGlobalTrajectoryInfo`](#classKinovaJaco2_1a83b6a3536c073edf0ca11f598728fd25) | Get trajectory information.
`public int(* `[`sendBasicTrajectory`](#classKinovaJaco2_1a3af6f854b026b4d689744cf891024ea1) | Send simple trajectory.
`public int(* `[`sendAdvanceTrajectory`](#classKinovaJaco2_1a91250787912ec137cc261e7c4181d6c1) | Send trajectory with constraints.
`public  `[`KinovaJaco2`](#classKinovaJaco2_1a676ea73535207e639228168d7b0bfa37)`()` | Kinova constructor.
`public  `[`~KinovaJaco2`](#classKinovaJaco2_1a3f4c40e06e5b881fc54e74d5d9d74310)`()` | Kinova destructor.
`public TrajectoryPoint `[`convertReferenceFrame`](#classKinovaJaco2_1ae9f3335993c29acf11d02ce0091a80bf)`(CartesianPosition & pose,TrajectoryPoint & cmdIn)` | Convert reference frame to base frame.
`public void `[`sendHomeTrajectory`](#classKinovaJaco2_1a3d8c9918b81b9a2e222e686ff7b49595)`()` | Move to home position.
`public void `[`sendRetractedTrajectory`](#classKinovaJaco2_1a7b65f584f5dc0302d698196fc50f110f)`()` | Move to retracted position.
`public void `[`sendScheduledTrajectory`](#classKinovaJaco2_1a22e03acd898ca97ca0ee783c3a46925f)`(TrajectoryPoint trajectory)` | Send and schedule trajectory.
`public bool `[`isAtTarget`](#classKinovaJaco2_1ab418f2914b695933a53bac2c1a77402e)`(TrajectoryPoint target_trajectory)` | Target status.
`public void `[`sleepMs`](#classKinovaJaco2_1a914a261fac82dc9ff2e8b1d596f2fd5a)`(int ms)` | Sleep milliseconds.
`protected int(* `[`initAPI`](#classKinovaJaco2_1ad432400048a81156df1774ee17e4764c) | Initializes API (USB)
`protected int(* `[`initEthernetAPI`](#classKinovaJaco2_1a5ad86fcee6afa4e42a369f856abb37d3) | Initializes API (Ethernet)
`protected int(* `[`closeAPI`](#classKinovaJaco2_1ad3017ccb582ca78b7826c4016a458859) | Close API.

## Members

#### `public int(* `[`initHome`](#classKinovaJaco2_1a425ca9acefe1571e4ced89b859b50fb1) 

Calibration routine for arm.

Moves the arm to a predefined home position for calibration purposes. Provided by Kinova.

#### Returns
Error code

#### `public int(* `[`initFingers`](#classKinovaJaco2_1a9c1405d6669a50f41f52a9820035f216) 

Calibration routine for fingers.

Moves the fingers to a predefined home position for calibration purposes. Provided by Kinova.

#### Returns
Error code

#### `public int(* `[`refresDevicesList`](#classKinovaJaco2_1a87f83b83ae1d298ed6fc56c926b04826) 

Refreshes device list.

Call to refresh the Kinova device list. Provided by Kinova.

#### Returns
Error code

#### `public int(* `[`getDevices`](#classKinovaJaco2_1a764a8497c6e804dc357926243329d3ab) 

Get a list of devices.

Establishes an API connection and list currently connected devices. The device list is not returned but rather a parameter reference. Provided by Kinova.

#### Parameters
* `KinovaDevice[]` Empty device list 

* `result` Value for apiLayer 

#### Returns
Error code

#### `public int(* `[`setActiveDevice`](#classKinovaJaco2_1a4f1954df31d33fb5bd8bfee3344b5d48) 

Select a device.

Is used in combination with refreshDeviceList() and [getDevices()](#classKinovaJaco2_1a764a8497c6e804dc357926243329d3ab) in order to select a Kinova device if more then one is atteched. Provided by Kinova.

#### Parameters
* `device` Device to be used 

#### Returns
Error code

#### `public int(* `[`getControlType`](#classKinovaJaco2_1a4226acf96370a26f3eba2d31461ca627) 

Get type of control.

The response parameter is used to differentiate between cartesian and angular control mode. Provided by Kinova.

#### Parameters
* `Response` Return reference for control mode 

#### Returns
Error code

#### `public int(* `[`setCartesianControl`](#classKinovaJaco2_1aecef09554385d1fe4f48a875ffd0e018) 

Set cartesian control mode.

Set type of control to cartesian mode. Only possible if the robot is NOT in an invalid position. Provided by Kinova.

#### Returns
Error code

#### `public int(* `[`setAngularControl`](#classKinovaJaco2_1a84a9ea5b49dbe3ef33aa33a31b41df54) 

Set angular control mode.

Set type of control to angular mode. Might not be able to switch back to cartesian mode. Provided by Kinova.

#### Returns
Error code

#### `public int(* `[`getCartesianPosition`](#classKinovaJaco2_1afab6fbdde7d95c6e3667bd0b89b6ad78) 

Get cartesian position.

The CartesianPosition parameter is used to return the current position in cartesian coordinates. The cartesian coordinates are the real sensor coordinates. Provided by Kinova.

#### Parameters
* `pt` Return reference for cartesian position 

#### Returns
Error code

#### `public int(* `[`getCartesianCommand`](#classKinovaJaco2_1a7dd5153bdfa220e290c96910eea89764) 

Get cartesian position.

The CartesianPosition parameter is used to return the current position in cartesian coordinates. The cartesian coordinates are the desired user coordinates previously send to the robot. Provided by Kinova.

#### Parameters
* `pt` Return reference for cartesian position 

#### Returns
Error code

#### `public int(* `[`getAngularPosition`](#classKinovaJaco2_1aa98af0706e3a466d8ab9036c6cc7d0b3) 

Get angular position.

The AngularPosition parameter is used to return the current position in angular coordinates. The angular coordinates are the real sensor coordinates. Provided by Kinova.

#### Parameters
* `pt` Return reference for angular position 

#### Returns
Error code

#### `public int(* `[`getAngularCommand`](#classKinovaJaco2_1aba62e119d7720205c34356815610e83a) 

Get angular position.

The AngularPosition parameter is used to return the current position in angular coordinates. The angular coordinates are the desired user coordinates previously send to the robot. Provided by Kinova.

#### Parameters
* `pt` Return reference for angular position 

#### Returns
Error code

#### `public int(* `[`getGlobalTrajectoryInfo`](#classKinovaJaco2_1a83b6a3536c073edf0ca11f598728fd25) 

Get trajectory information.

The TrajectoryFIFO parameter is used to return the trajectory scheduler information. Can be used to time trajectory actions. Provided by Kinova.

#### Parameters
* `Response` Return reference for frajectory scheduler 

#### Returns
Error code

#### `public int(* `[`sendBasicTrajectory`](#classKinovaJaco2_1a3af6f854b026b4d689744cf891024ea1) 

Send simple trajectory.

Commands the robot to move to a certain position or with a certain velocity. The basic trajectory does not allow trajectory constraints. Provided by Kinova.

#### Parameters
* `trajectory` Trajectory to move to 

#### Returns
Error code

#### `public int(* `[`sendAdvanceTrajectory`](#classKinovaJaco2_1a91250787912ec137cc261e7c4181d6c1) 

Send trajectory with constraints.

Commands the robot to move to a certain position or with a certain velocity. Trajectory constraints can also be added. Provided by Kinova.

#### Parameters
* `trajectory` Trajectory to move to 

#### Returns
Error code

#### `public  `[`KinovaJaco2`](#classKinovaJaco2_1a676ea73535207e639228168d7b0bfa37)`()` 

Kinova constructor.

Creates an robot istance to control. All necessary library call and initializations of the Kinova API are implemented here.

#### `public  `[`~KinovaJaco2`](#classKinovaJaco2_1a3f4c40e06e5b881fc54e74d5d9d74310)`()` 

Kinova destructor.

Frees all Kinova related memory and libraries.

#### `public TrajectoryPoint `[`convertReferenceFrame`](#classKinovaJaco2_1ae9f3335993c29acf11d02ce0091a80bf)`(CartesianPosition & pose,TrajectoryPoint & cmdIn)` 

Convert reference frame to base frame.

Converts movements in a reference frame to corresponding motions in the robot base frame. Can be used to track object with a camera.

#### Parameters
* `pose` Current position 

* `cmdIn` Trajectory in reference frame 

#### Returns
Trajectory in base frame

#### `public void `[`sendHomeTrajectory`](#classKinovaJaco2_1a3d8c9918b81b9a2e222e686ff7b49595)`()` 

Move to home position.

Moves the robot to a user defined home position.

#### `public void `[`sendRetractedTrajectory`](#classKinovaJaco2_1a7b65f584f5dc0302d698196fc50f110f)`()` 

Move to retracted position.

Moves the robot to a user defined retracted (storage) position.

#### `public void `[`sendScheduledTrajectory`](#classKinovaJaco2_1a22e03acd898ca97ca0ee783c3a46925f)`(TrajectoryPoint trajectory)` 

Send and schedule trajectory.

Commands the robot to move to a certain position or with a certain velocity. Waits for the robot to reach the requested position.

#### Parameters
* `trajectory` Trajectory to move to

#### `public bool `[`isAtTarget`](#classKinovaJaco2_1ab418f2914b695933a53bac2c1a77402e)`(TrajectoryPoint target_trajectory)` 

Target status.

Compares a desired trajectory with the current position to check if target location was reached

#### Parameters
* `trajectory` Target trajectory 

#### Returns
true if at target location

#### `public void `[`sleepMs`](#classKinovaJaco2_1a914a261fac82dc9ff2e8b1d596f2fd5a)`(int ms)` 

Sleep milliseconds.

#### Parameters
* `ms` Milliseconds to sleep

#### `protected int(* `[`initAPI`](#classKinovaJaco2_1ad432400048a81156df1774ee17e4764c) 

Initializes API (USB)

Initializes the Kinova USB API. USB is default for Kinova.

#### Returns
API Layer information used for connection

#### `protected int(* `[`initEthernetAPI`](#classKinovaJaco2_1a5ad86fcee6afa4e42a369f856abb37d3) 

Initializes API (Ethernet)

Initializes the Kinova API for ethernet usage

#### Returns
API Layer information used for connection

#### `protected int(* `[`closeAPI`](#classKinovaJaco2_1ad3017ccb582ca78b7826c4016a458859) 

Close API.

Free API layer and related memory

#### Returns
Error code

# class `PIDController` 

A simple PID controller class.

A PID controller that only provides simple functions.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`PIDController`](#classPIDController_1ac004d8c72b74c646de9ad69dae4e09e9)`(double Kp,double KTi,double KTd)` | PID Constructor.
`public  `[`~PIDController`](#classPIDController_1a690e7ad4796e5c5143aa4b90f2f6677b)`()` | PID destructor.
`public void `[`setTimeMode`](#classPIDController_1a0f435928dd3eca1061eae7550060b049)`()` | Set time based calculation.
`public void `[`setGainMode`](#classPIDController_1a33de6d08c5313c5056b49afd7532bb9d)`()` | Set time based calculation.
`public double `[`calculate`](#classPIDController_1af31656b67630f25fe8b85eefa24fe956)`(double e)` | Calculates controll unit value.

## Members

#### `public  `[`PIDController`](#classPIDController_1ac004d8c72b74c646de9ad69dae4e09e9)`(double Kp,double KTi,double KTd)` 

PID Constructor.

Creates a PID controller for universal usage Depending on the controller mode (time base or gain based) the second and third parameter very in function.

#### Parameters
* `Kp` proportional gain 

* `KTi` integral gain / integral time 

* `KTd` derivation gain / derivation time

#### `public  `[`~PIDController`](#classPIDController_1a690e7ad4796e5c5143aa4b90f2f6677b)`()` 

PID destructor.

Frees all PID related memory.

#### `public void `[`setTimeMode`](#classPIDController_1a0f435928dd3eca1061eae7550060b049)`()` 

Set time based calculation.

Sets the calculation mode to time based. u = Kp( e + 1/Ti (e_sum) + Td (e-e_old))

#### `public void `[`setGainMode`](#classPIDController_1a33de6d08c5313c5056b49afd7532bb9d)`()` 

Set time based calculation.

Sets the calculation mode to gain based. u = Kp e + Ki (e_sum) + Kd (e-e_old) Gain based by default

#### `public double `[`calculate`](#classPIDController_1af31656b67630f25fe8b85eefa24fe956)`(double e)` 

Calculates controll unit value.

Calculates the controll value u depending on the error e. Gain based: u = Kp e + Ki T (e_sum) + Kd/T (e-e_old) Time based: u = Kp e + Ki (e_sum) + Kd (e-e_old) Gain based by default

#### Parameters
* `e` error value 

#### Returns
controll value

# class `TCPClient` 

A simple TCP client class.

A TCP client that provides simple but robust functionalities for networking.

## Summary

 Members                        | Descriptions                                
--------------------------------|---------------------------------------------
`public  `[`TCPClient`](#classTCPClient_1a7001db08f8f54bfce97ed3eca11d2ffa)`(std::string ip,int port)` | Client constructor.
`public  `[`~TCPClient`](#classTCPClient_1a869a5b3319ca562d03cb4c59ebec4407)`()` | Client destructor.
`public bool `[`isConnected`](#classTCPClient_1a698c83ee3c90ac82c321318bb846267e)`()` | Connection verification.
`public void `[`send`](#classTCPClient_1ada1ef042c0ed55d42ea2cea18733cb9e)`(std::string msg)` | Sends message to server.
`public std::string `[`receive`](#classTCPClient_1a316a74b789c78f26c48df434dca80ee5)`(int buff_size)` | Receives message from server.

## Members

#### `public  `[`TCPClient`](#classTCPClient_1a7001db08f8f54bfce97ed3eca11d2ffa)`(std::string ip,int port)` 

Client constructor.

Creates and initializes the necessary sockets, file descriptors and connections.

#### Parameters
* `ip` Network IP address 

* `port` Network server port

#### `public  `[`~TCPClient`](#classTCPClient_1a869a5b3319ca562d03cb4c59ebec4407)`()` 

Client destructor.

Frees all client related memory.

#### `public bool `[`isConnected`](#classTCPClient_1a698c83ee3c90ac82c321318bb846267e)`()` 

Connection verification.

Checks if the connection is still activ.

#### Returns
If client is connected to a server

#### `public void `[`send`](#classTCPClient_1ada1ef042c0ed55d42ea2cea18733cb9e)`(std::string msg)` 

Sends message to server.

Sends a message from client to server while providing error and fault detection.

#### Parameters
* `msg` Message to send

#### `public std::string `[`receive`](#classTCPClient_1a316a74b789c78f26c48df434dca80ee5)`(int buff_size)` 

Receives message from server.

Receives a message from server while providing error and fault detection.

#### Parameters
* `buff_size` Buffer size for incoming messages 

#### Returns
Received message

Generated by [Moxygen](https://sourcey.com/moxygen)