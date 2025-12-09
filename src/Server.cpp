#include "Server.hpp"

Server::Server()
{
	this->_signal = false;
	this->_port = 0;
	this->_serverSocketFd = -1;
}

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

std::string Server::getPassword()
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

void	Server::setFd(int fd)
{
	this->_serverSocketFd = fd;
}

void	Server::setPort(int port)
{
	this->_port = port;
}

void Server::setPassword(std::string pw)
{
	this->_password = pw;
}

void Server::serverInit()
{
	this->socketInit();

	std::cout << "Server <" << _serverSocketFd << "> Connected" << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (Server::_signal == false)
	{
		if((poll(&_clientSocketFds[0],_clientSocketFds.size(),-1) == -1) && Server::_signal == false)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < _clientSocketFds.size(); i++)
		{
			if (_clientSocketFds[i].revents & POLLIN)
			{
				if (_clientSocketFds[i].fd == _serverSocketFd)
					this->acceptNewClient();
				else
					this->receiveNewData(_clientSocketFds[i].fd);
			}
		}
	}
	closeFds();
}

void Server::socketInit()
{
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr .s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->_port);
	this->_serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;

	if (this->_serverSocketFd == -1)
		throw (std::runtime_error("Failed to create server socket"));

	if (setsockopt(_serverSocketFd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) < 0 ))
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
	{
		std::cout << "accept() failed." << std::endl;
		return;
	}
	if (fcntl(newFd, F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << "ftcnl() nonblock failed." << std::endl;
		return;
	}

	Client client;
	memset(&clientAddress, 0, sizeof(clientAddress));
	socklen_t len = sizeof(clientAddress);
	newClient.fd = newFd;
	newClient.events = POLLIN;
	newClient.revents = 0;
	client.setFd(newFd);
	// client.setIpAdd(inet_ntoa((clientAddress.sin_addr)));
	_clients.push_back(client);
	_clientSocketFds.push_back(newClient);

}

void	Server::receiveNewData(int fd)
{
	char buffer[1024];
	bzero(buffer, sizeof(buffer));
	ssize_t  bytes = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes == -1)
		std::cout << "recv() failed." << std::endl;
	else if (bytes == 0)
		endConnection(fd);
	else
		handleMessage(fd, buffer);
}

void	endConnection(int fd)
{

}

void Server::removeFds(int fd)
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

void	handleMessage(int fd, char *buffer)
{
	
}

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << "Signal Received!" << std::endl;
	Server::_signal = true;
}
