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
#include "Replies.hpp"

class Client;
class Channel;

class Server
{
	private:
		int							_maxFd;
		int							_port;
		int							_reserveFd;
		int							_serverSocketFd;
		static bool						_signal;
		std::string					_password;
		std::vector<Client>			_clients;
		std::vector<Channel>		_channels;
		std::vector<struct pollfd>	_clientSocketFds;
		std::map<int, std::string> _clientBuffers;
		struct sockaddr_in			serverAddress;
		struct sockaddr_in			clientAddress;
		struct pollfd				newClient;

	public:

		// default constructor
		Server();
		// copy constructor
		Server(Server const &og);
		// copy assignement operator
		Server&		operator=(Server const &og);
		// default destructor
		~Server();

		// getters
			int			getSocketFd();
			int			getPort();
			std::string	getPassword();
			Client*		getClientFd(int fd);
			Client*		getClientNick(std::string nickname);
			Channel*	getChannel(std::string name);

		// setters
			void		setFd(int fd);
			void		setPort(int port);
			void		setPassword(std::string pw);

		// server methods
			/*Initializes and runs the main server loop.
			Sets up the server socket, waits for incoming client connections,
			and handles client data using poll(). Accepts new clients or processes
			data from existing clients until a termination signal is received.
			Cleans up and closes all sockets on exit.*/
			void		serverInit();
			/* Creates a non-blocking TCP socket, sets socket options, binds it to the specified port,
			and starts listening for client connections. Adds the server socket to the pollfd list. */
			void		socketInit();
			/* Accepts an incoming connection on the server socket, sets the new client socket to non-blocking mode,
			creates a new Client object, and adds it to the client and pollfd lists. Prints connection info to stdout.
			If the maximum number of clients is reached, prints a warning. */
			void		acceptNewClient();
			/* Reads data from the specified client socket. If data is received, appends it to the client's buffer,
			splits complete messages, and parses each message. Handles client disconnection and errors. */
			void		receiveNewData(int fd);
			/* Splits the received messages by /n new line characters */
			std::vector<std::string>	splitMessage(std:: string message);

		// remove methods
			/* Removes clientSocketFds from the container */
			void		removeFds(int fd);
			/* Remove client from the list of clients */
			void		removeClient(int fd);
			/* Removes chanels from container*/
			void		removeChannel(std::string name);
			/* Removes the client socket from the pollfd and client lists, closes the socket,
			prints a disconnect message, and updates the client count. */
			void		endConnection(int fd);

		// send
			/* Sends response to IRC client */
			void		sendRsp(std::string msg, int fd);

		// close methods
			void		closeFds();

		// signal methods
			static void	signalHandler(int signum);

		// parsing
			/* Splits the received message by spaces */
			std::vector<std::string>	splitCmd(std::string &str);
			/* Verifies if the client is registered on the server */
			bool		registered(int &fd);
			/* Parses the message by command */
			void		parseMessage(std::string &cmd, int &fd);

		// pass cmd
			void		pass(std::vector<std::string> &, int &);

		// nick cmd
			bool		validNick(std::string& nickName);
			bool		nickInUse(std::string& nickName);
			void		nick(std::vector<std::string> &, int &);

		// part cmd
			void		splitChannelPart(std::vector<std::string> &channPart, std::string &temp);
			bool		splitPart(std::vector<std::string> &tokens, std::vector<std::string> &channPart, std::string &reason, int fd);
			void		part(std::vector<std::string> &, int &);

		// privmsg cmd
			void		splitRecipients(std::vector<std::string> &recipients, std::vector<std::string> &channPrivmsg, std::string &temp);
			void		splitPrivmsg(std::vector<std::string> &tokens, std::vector<std::string> &recipients, std::vector<std::string> &channPrivmsg, std::string &msg, int fd);
			void		privmsg(std::vector<std::string> &, int &);

		// quit cmd
			std::string	splitQuit(std::vector<std::string> &tokens);
			void		quit(std::vector<std::string> &, int &);

		// topic cmd
			int			splitTopic(std::vector<std::string> &tokens, std::string &chanName, std::string &topic);
			void		topic(std::vector<std::string> &, int &);

		// user cmd
			void		user(std::vector<std::string> &, int &);

		// invite cmd
			void		invite(std::vector<std::string> &, int &);

		// join cmd
			int			searchClientInChannels(std::string nick);
			void		channelNotExist(std::vector<std::pair<std::string, std::string> > token, size_t i, int fd);
			void		channelExist(std::vector<std::pair<std::string, std::string> > token, size_t i, size_t j, int fd);
			int			splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::vector<std::string> &cmd, int fd);
			void		join(std::vector<std::string> &, int &);

		// kick cmd
			void		kick(std::vector<std::string> &, int &);

		// mode cmd
			std::string	limitMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain);
			std::string	operatorPrivilegeMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain);
			std::string	passwordMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain);
			void		mode(std::vector<std::string> &, int &);

};

// aux
std::string		currentTime();
std::string		findMsg(std::vector<std::string> &tokens);
typedef void	(Server::*CmdFuncs)(std::vector<std::string>&, int&);
