#include "TCPClient.hpp"


TCPClient::TCPClient(std::string ip, int port)
{
	// create an socket 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	isActive = true;

	// check if a socket was created
	if (sockfd == -1)
	{
		std::cout << "[WARNING] client socket creation failed" << std::endl;
		isActive = false;
		return;
	}

	// define server address to connect to
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(ip.c_str());
	servaddr.sin_port = htons(port);

	// connect to server
	if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0) 
	{
		std::cout << "[WARNING] connection to server failed" << std::endl;
		isActive = false;
		return;
	} 

	int n = setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*) 1, 0);
}


TCPClient::~TCPClient()
{
	isActive = false;

	// close socket
	close(sockfd);
}


bool TCPClient::isConnected()
{
	// get options and error code of client socket
	int error = 0;
	unsigned int len = sizeof (error);
	int retval = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len);

	isActive = !(sockfd <= 0 || retval != 0 || error != 0 || !isActive);

	return isActive;
}


void TCPClient::send(std::string msg)
{
	// write string to client socket 
	int n = write(sockfd, msg.c_str(), msg.length()+1);

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


std::string TCPClient::receive(int buff_size)
{
	// read buffer
	char buffer[buff_size];
	bzero(&buffer, buff_size);

	// read message from client socket
	int n = read(sockfd, buffer, buff_size);

	// check if no bytes where received
	if (n <= 0)
	{
		std::cout << "[WARNING] transmission closed" << std::endl;
		isActive = false;
	}
	
	return std::string(buffer);
}
