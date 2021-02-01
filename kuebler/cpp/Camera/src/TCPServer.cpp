#include "TCPServer.hpp"

TCPServer::TCPServer(int port)
{
	// create an socket 
    	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	isActive = true;

	// check if a socket was created
    	if (sockfd == -1) 
	{
        	std::cout << "[WARNING] server socket creation failed" << std::endl;
		isActive = false;
	}

	// define server address to bind to
    	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	// bind server socket to address
    	if ((bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))) != 0) 
	{
		std::cout << "[WARNING] server socket bind failed" << std::endl;
		isActive = false;
		return;
    	}
    
	// listen for incoming connection
	if ((listen(sockfd, 5)) != 0)
	{
		std::cout << "[WARNING] socket listen failed" << std::endl;
		isActive = false;
		return;
	}

	// accept client socket for communication
	unsigned int len = sizeof(cli);
    	if ((connfd = accept(sockfd, (sockaddr*)&cli, &len)) < 0) 
	{ 
		std::cout << "[WARNING] client socket accept failed" << std::endl;
		isActive = false;
		return;
   	}    

	// apply socket options
	// send heartbeat for connection validation
	int n = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) 1, 0);
	
}


TCPServer::~TCPServer()
{
	isActive = false;

	// close all sockets
	close(sockfd);
	close(connfd);
}


bool TCPServer::isConnected()
{
	// get options and error code of client socket
	int error = 0;
	unsigned int len = sizeof (error);
	int retval = getsockopt (connfd, SOL_SOCKET, SO_ERROR, &error, &len);

	// check for all sockets, and their error codes
	isActive = !(sockfd <= 0 || connfd <= 0 || retval != 0 || error != 0 || !isActive);

	return isActive;
}


void TCPServer::send(std::string msg)
{
	// write string to client socket 
	int n = write(connfd, msg.c_str(), msg.length()+1);

	// check if nothing or only parts of the message where written to client socket
	if (n <=0)
	{
		std::cout << "[WARNING] transmission closed" << std::endl;
		isActive = false;
	}
 	else if (n < msg.length())
	{
		std::cout << "[WARNING] transmission fault" << std::endl;
		isActive = false;
	}
	
}


std::string TCPServer::receive(int buff_size)
{
	// read buffer
	char buffer[buff_size];
	bzero(&buffer, buff_size);

	// read message from client socket
	int n = read(connfd, buffer, buff_size);

	// check if no bytes where received
	if (n <= 0)
	{
		std::cout << "[WARNING] transmission closed" << std::endl;
		isActive = false;
	}
	
	return std::string(buffer);	
}
