#ifndef _TCPCLIENT_HPP_
#define _TCPCLIENT_HPP_

#include <stdio.h>
#include <strings.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>


/**
* A simple main function to demonstrate the creation and usage of a client socket.
* In this example the client only listens to the server and receives all the servers messages.
* Best used in combination with "TEST_TCPServer". 
*
* @example TEST_TCPClient.cpp
*/

/**
* A more complex example to demonstrate the interaction between TCPClient and the Kinova Jaco.
* Messages are received and parsed for movement in x and y direction only.
*
* @example TEST_TCPClient_KinovaJaco.cpp
*/

/**
* A TCPClient demonstration including JSON.
* Messages are received and parsed using the JSON format.
*
* @example TEST_TCPClient_Jason.cpp
*/

/**
* @brief A simple TCP client class
*
* A TCP client that provides simple but robust functionalities for networking.
*/
class TCPClient
{
	private:

		int sockfd; /**< File descriptor for socket */
		
		struct sockaddr_in servaddr; /**< Socket address of server socket */
		struct sockaddr_in cli; /**< Socket address of client socket */

		bool isActive;/**< Connection status */

	
	public:
		/**
		* @brief Client constructor
		*
		* Creates and initializes the necessary sockets, file descriptors and connections. 
		*
		* @param ip Network IP address
		* @param port Network server port
		*/
		TCPClient(std::string ip, int port);

		/**
		* @brief Client destructor
		*
		* Frees all client related memory.
		*/
		~TCPClient();

		/**
		* @brief Connection verification
		*
		* Checks if the connection is still activ.
		*
		* @return If client is connected to a server
		*/
		bool isConnected();

		/**
		* @brief Sends message to server
		*
		* Sends a message from client to server while providing error and fault detection.
		*
		* @param msg Message to send
		*/
		void send(std::string msg);

		/**
		* @brief  Receives message from server
		*
		* Receives a message from server while providing error and fault detection.
		*
		* @param buff_size Buffer size for incoming messages
		* @return Received message
		*/
		std::string receive(int buff_size);

};

#endif
