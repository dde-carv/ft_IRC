#pragma once

#include <iostream>
#include <cstdlib>
#include <vector>			//-> for vector container
#include <map>				//-> for map container
#include <sys/socket.h>		//-> for socket()
#include <sys/types.h>		//-> for socket()
#include <netinet/in.h>		//-> for sockaddr_in
#include <unistd.h>			//-> for close()
#include <arpa/inet.h>		//-> for inet_ntoa()
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <csignal>			//-> for signals
#include <fstream>			//-> for stream files
#include <ctime>			//-> for time stamps

class Client;
class Channel;

class Server
{
	private:
		int _port;	//-> server port
		int _socketFd;	//-> server socket file descriptor
		bool _signal;
		std::string _password;	//->
		// std::vector<Client> _clients;	//-> vector of clients
		// std::vector<Channel> _channels;	//-> vector of channels
		std::vector<struct pollfd> _fds;	//-> vector of fds
		struct sockaddr_in serverAddress;
		// struct sockaddr_in clientAddress;
		struct pollfd newClient;

	public:
		// default constructor
		Server();
		// copy constructor
		Server(Server const &Server);
		// copy assignement operator
		Server&	operator=(Server const &Server);
		// default destructor
		~Server();

		// getters
		int	getSocketFd();
		int	getPort();
		std::string getPassword();

		// setters
		void	setFd(int fd);
		void	setPort(int port);
		void	setPassword(std::string pw);

		// server methods
		void serverInit();
		void socketInit();

		// remove methods
		void	removeFds(int fd);

		// close methods
		void	closeFds();

};
