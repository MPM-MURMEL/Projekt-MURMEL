#ifndef _TCPSERVER_HPP_
#define _TCPSERVER_HPP_

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
* A simple main function to demonstrate the creation and usage of a server socket.
*
* @example TEST_TCPServer.cpp
*/

/**
* @brief A simple TCP server class.
*
* A TCP server that provides simple but robust functionalities for networking.
*/
class TCPServer
{
	private:

 		int sockfd;  /**< File descriptor for server socket */
		int connfd;  /**< File descriptor for client socket */

		bool isActive; /**< Connection status */
	
    		struct sockaddr_in servaddr; /**< Socket address of server socket */
		struct sockaddr_in cli; /**< Socket address of client socket */
	
	public:
		/**
		* @brief Server constructor
		*
		* Creates and initializes all necessary sockets, file descriptors and connections. 
		*
		* @param port Network server port
		*/
		TCPServer(int port);

		/**
		* @brief Server destructor
		*
		* Frees all server related memory.
		*/
		~TCPServer();

		/**
		* @brief Connection verification
		*
		* Checks if the connection is still activ.
		*
		* @return If server is connected to a client
		*/
		bool isConnected();

		/**
		* @brief Sends message to client
		*
		* Sends a message from server to client while providing error and fault detection.
		*
		* @param msg Message to send
		*/
		void send(std::string msg);

		/**
		* @brief  Receives message from client
		*
		* Receives a message from client while providing error and fault detection.
		*
		* @param buff_size Buffer size for incoming messages
		* @return Received message
		*/
		std::string receive(int buff_size);

};


#endif
