#pragma once

#include <iostream>
#include <fstream>			//-> for stream files
#include <unistd.h>			//-> for close()
#include <string.h>
#include <cstdlib>
#include <csignal>			//-> for signals
#include <ctime>			//-> for time stamps
#include <vector>			//-> for vector container
#include <map>				//-> for map container
#include <sys/socket.h>		//-> for socket()
#include <sys/types.h>		//-> for socket()
#include <netinet/in.h>		//-> for sockaddr_in
#include <arpa/inet.h>		//-> for inet_ntoa()
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>

#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server
{
	private:
		int _port;	//-> server port
		int _serverSocketFd;	//-> server socket file descriptor
		static bool _signal;
		std::string _password;	//->
		std::vector<Client> _clients;	//-> vector of clients
		std::vector<Channel> _channels;	//-> vector of channels
		std::vector<struct pollfd> _clientSocketFds;	//-> vector of fds
		struct sockaddr_in serverAddress;
		struct sockaddr_in clientAddress;
		struct pollfd newClient;

	public:
		// default constructor
		Server();
		// copy constructor
		Server(Server const &og);
		// copy assignement operator
		Server&	operator=(Server const &og);
		// default destructor
		~Server();

		// getters
		int	getSocketFd();
		int	getPort();
		std::string getPassword();

		Client*	getClientFd(int fd);
		Client*	getClientNick(std::string nickname);
		Channel*	getChannel(std::string name);

		// setters
		void	setFd(int fd);
		void	setPort(int port);
		void	setPassword(std::string pw);

		// server methods
		void	serverInit();
		void	socketInit();
		void	acceptNewClient();
		void	receiveNewData(int fd);
		void	handleMessage(int fd, char *buffer);

		// remove methods
		void	removeFds(int fd);
		void	endConnection(int fd);

		// close methods
		void	closeFds();

		// signal methods
		void	signalHandler(int signum);

};
