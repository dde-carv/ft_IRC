#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
		int	_port;
		int	_serverSocketFd;
		static bool	_signal;
		std::string	_password;
		std::vector<Client>	_clients;
		std::vector<Channel>	_channels;
		std::vector<struct pollfd>	_clientSocketFds;
		struct sockaddr_in	serverAddress;
		struct sockaddr_in	clientAddress;
		struct pollfd	newClient;

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
		void	removeClient(int fd);
		void	removeChannel(std::string name);
		// void	removeChannels(int fd);
		void	endConnection(int fd);

		// close methods
		void	closeFds();

		// signal methods
		void	signalHandler(int signum);

};
