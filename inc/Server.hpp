#pragma once

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <ctime>
#include <vector>
#include <map>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <exception>
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server
{
	private:
		int _maxFd;
		int	_port;
		int	_reserveFd;
		int	_serverSocketFd;
		bool	_signal;
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
		// void	handleMessage(int fd, char *buffer);
		void	endConnection(int fd);

		// remove methods
		void	removeFds(int fd);
		void	removeClient(int fd);
		void	removeChannel(std::string name);
		// void	removeChannels(int fd);
		// void	endConnection(int fd);

		// send
		void	senderr(int code, std::string clientname, int fd, std::string msg);
		void	senderr(int code, std::string clientname, std::string channelname, int fd, std::string msg);
		void	sendRsp(std::string msg, int fd);

		// close methods
		void	closeFds();

		// signal methods
		void	signalHandler(int signum);

		// parsing
		std::vector<std::string>	splitCmd(std::string &str);

		// join cmd
		int		searchClientInChannels(std::string nick);
		void	channelExist(std::vector<std::pair<std::string, std::string> > token, size_t i, size_t j, int fd);
		int		splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd);
		void	join(std::string cmd, int fd);

		// invite cmd
		void	invite(std::string &cmd, int &fd);
};
