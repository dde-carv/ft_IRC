#include "Server.hpp"

bool Server::_signal = false;

Server::Server() : _maxFd(2), _reserveFd(-1), _serverSocketFd(-1), _password("")
{}

Server::Server(Server const &og)
{
	*this = og;
}

Server&	Server::operator=(Server const &og)
{
	if (this != &og)
	{
		this->_port = og._port;
		this->_serverSocketFd = og._serverSocketFd;
		this->_clientSocketFds= og._clientSocketFds;
		this->_password = og._password;
	}
	return *this;
}

Server::~Server()
{}

// getters

int	Server::getSocketFd()
{
	return this->_serverSocketFd;
}

int	Server::getPort()
{
	return this->_port;
}

std::string	Server::getPassword()
{
	return this->_password;
}

Client*	Server::getClientFd(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
			return &this->_clients[i];
	}
	return NULL;
}

Client*	Server::getClientNick(std::string nickname)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNickName() == nickname)
			return &this->_clients[i];
	}
	return NULL;
}

Channel*	Server::getChannel(std::string name)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == name)
			return &_channels[i];
	}
	return NULL;
}

// setters

void	Server::setFd(int fd)
{
	this->_serverSocketFd = fd;
}

void	Server::setPort(int port)
{
	this->_port = port;
}

void	Server::setPassword(std::string pw)
{
	this->_password = pw;
}

// server methods

void	Server::serverInit()
{
	this->socketInit();

	std::cout << "Server <" << _serverSocketFd << "> Connected" << std::endl;
	std::cout << "Waiting to accept a connection...\n";

	while (Server::_signal == false)
	{
		if((poll(&_clientSocketFds[0],_clientSocketFds.size(), -1) == -1) && Server::_signal == false)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < _clientSocketFds.size(); i++)
		{
			if (_clientSocketFds[i].revents & POLLIN)
			{
					if (_clientSocketFds[i].fd == _serverSocketFd && _maxFd < 1021)
						this->acceptNewClient();
					else
						this->receiveNewData(_clientSocketFds[i].fd);
			}
		}
	}
	closeFds();
}

void	Server::socketInit()
{
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr .s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->_port);
	this->_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;

	if (this->_serverSocketFd == -1)
		throw (std::runtime_error("Failed to create server socket"));

	if (setsockopt(_serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
		throw (std::runtime_error("Failed to set option (SP_REUSEADDR) on socket"));

	if (fcntl(_serverSocketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));

	if (bind(_serverSocketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
		throw (std::runtime_error("Failed to bind"));

	if (listen(_serverSocketFd, SOMAXCONN) == -1)
		throw (std::runtime_error("Failed to listen"));

	newClient.fd = _serverSocketFd;
	newClient.events = POLLIN;
	newClient.revents = 0;
	_clientSocketFds.push_back(newClient);
}

void	Server::acceptNewClient()
{
	int newFd = accept(_serverSocketFd, NULL, NULL);

	if (newFd == -1)
		throw (std::runtime_error("accept() failed."));
	if (fcntl(newFd, F_SETFL, O_NONBLOCK) == -1)
		throw (std::runtime_error("ftcnl() nonblock failed."));
	this->_maxFd++;
	std::cout << "New client: " << newFd << std::endl;
	if (this->_maxFd > 1020)
		std::cout << "Max number of clients reached" << std::endl;

	Client client;
	memset(&clientAddress, 0, sizeof(clientAddress));
	newClient.fd = newFd;
	newClient.events = POLLIN;
	newClient.revents = 0;
	client.setFd(newFd);
	client.setIpAdd(inet_ntoa((clientAddress.sin_addr)));
	_clients.push_back(client);
	_clientSocketFds.push_back(newClient);

}

void	Server::receiveNewData(int fd)
{
	char buffer[512];
	bzero(buffer, sizeof(buffer));
	ssize_t  bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	Client *cli = getClientFd(fd);
	if (_maxFd < 1020)
	{
		if (bytes == -1)
			throw (std::runtime_error("recv() failed."));
		else if (bytes == 0)
			endConnection(fd);
		else
		{
			cli->setBuffer(buffer);
			if(cli->getBuffer().find_first_of("\r\n") == std::string::npos)
				return;
			std::vector<std::string> splitedmessage = splitMessage(cli->getBuffer());
			for(size_t i = 0; i < splitedmessage.size(); i++)
				parseMessage(splitedmessage[i], fd);
			if(getClientFd(fd))
				getClientFd(fd)->clearBuffer();
		}
	}
	else if (bytes == 0)
			endConnection(fd);
}

void	Server::endConnection(int fd)
{
	for (std::vector<pollfd>::iterator i = _clientSocketFds.begin(); i != _clientSocketFds.end(); i++)
	{
		if (i->fd == fd)
		{
			_clientSocketFds.erase(i);
			removeClient(fd);
			close(fd);
			std::cout << "Connection closed: " << fd << std::endl;
			_maxFd--;
			break ;
		}
	}
}

std::vector<std::string>	Server::splitMessage(std::string message)
{
	std::vector<std::string> splitmessage;
	std::string temp;

	for (size_t i = 0; i < message.size(); i++)
	{
		size_t y = message.find('\n', i);
		if (y != std::string::npos)
		{
			splitmessage.push_back(message.substr(i, y - i));
			i = y;
			temp.clear();
		}
	}
	return (splitmessage);
}

// remove methods

void	Server::removeFds(int fd)
{
	for (size_t i = 0; i < this->_clientSocketFds.size(); i++)
	{
		if (this->_clientSocketFds[i].fd == fd)
			{
				this->_clientSocketFds.erase(this->_clientSocketFds.begin() + i);
				return ;
			}
	}
}

void	Server::removeClient(int fd)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getFd() == fd)
			{
				this->_clients.erase(this->_clients.begin() + i);
					return ;
			}
	}
}

void	Server::removeChannel(std::string name)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == name)
			{
				this->_channels.erase(this->_channels.begin() + i);
					return ;
			}
	}
}

// send

void	Server::sendRsp(std::string msg, int fd)
{
	if (send(fd, msg.c_str(), msg.size(), 0) == -1)
		throw (std::runtime_error("send() failed"));
}

// close methods

void	Server::closeFds()
{
	for (size_t i = 0; i < _clients.size(); i++)
	{
		std::cout << "Client <" << _clients[i].getFd() << "> Disconnected" << WHI << std::endl;
		close(_clients[i].getFd());
	}
	if (_serverSocketFd != -1)
	{
		std::cout << "Server <" << _serverSocketFd << "> Disconnected" << std::endl;
		close(_serverSocketFd);
	}
}

// signal methods

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << "Signal Received!" << std::endl;
	Server::_signal = true;
}

// parsing methods

std::vector<std::string> Server::splitCmd(std::string &str)
{
	std::vector<std::string> vec;
	std::istringstream stm(str);
	std::string token;
	while(stm >> token)
	{
		vec.push_back(token);
		token.clear();
	}
	return vec;
}

bool	Server::registered(int &fd)
{
	if (!this->getClientFd(fd) || this->getClientFd(fd)->getNickName().empty() || this->getClientFd(fd)->getUserName().empty())
		return false;
	return true;
}

void	Server::parseMessage(std::string &cmd, int &fd)
{
	std::vector<std::string> tokens = splitCmd(cmd);
	std::string cmdUpper[] = {"USER", "PASS", "QUIT", "NICK", "INVITE", "JOIN", "KICK", "MODE", "PART", "PRIVMSG", "TOPIC"};
	std::string cmdLower[] = {"user", "pass", "quit", "nick", "invite", "join", "kick", "mode", "part", "privmsg", "topic"};
	size_t sizeCmdNotRegistered = 4;
	size_t sizeCmdList = sizeof(cmdUpper) / sizeof(cmdUpper[0]);
	CmdFuncs cmdFuncs[] = { &Server::user, &Server::pass, &Server::quit, &Server::nick, &Server::invite, &Server::join, \
	&Server::kick, &Server::mode, &Server::part, &Server::privmsg, &Server::topic};

	for (size_t i = 0; i < tokens.size(); i++)
		std::cout << "splitted cmd " << i << " is " << tokens[i] << std::endl;

	if (tokens.size())
	{
		for (size_t i = 0; i < sizeCmdNotRegistered; i++)
		{
			if (!registered(fd) && (tokens[0] == cmdUpper[i] || tokens[0] == cmdLower[i]))
				return (this->*cmdFuncs[i])(tokens, fd);
		}
		for (size_t i = 2; i < sizeCmdList; i++)
		{
			if (registered(fd) && (tokens[0] == cmdUpper[i] || tokens[0] == cmdLower[i]))
				return (this->*cmdFuncs[i])(tokens, fd);
			else if ( i == 10)
				return sendRsp(ERR_UNKNOWNCOMMAND(this->getClientFd(fd)->getNickName(), tokens[0]), fd);
		}
		sendRsp(ERR_NOTREGISTERED(std::string("*")), fd);
	}
}
