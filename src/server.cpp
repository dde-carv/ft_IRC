#include "server.hpp"

Server::Server()
{
	_signal = false;
	this->_port = 0;
	this->_socketFd = -1;
}

Server::Server(Server const &og)
{
	*this = og;
}

Server&	Server::operator=(Server const &og)
{
	if (this != &og)
	{
		this->_socketFd = og._socketFd;
	}
	return *this;
}

Server::~Server()
{}

int	Server::getSocketFd()
{
	return this->_socketFd;
}

int	Server::getPort()
{
	return this->_port;
}

std::string Server::getPassword()
{
	return this->_password;
}

void	Server::setFd(int fd)
{
	this->_socketFd = fd;
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

	std::cout << "Server <" << _socketFd << "> Connected" << std::endl;
	std::cout << "Waiting to accept a connection...\n";
	while (Server::_signal == false)
	{
		if((poll(&_fds[0],_fds.size(),-1) == -1) && Server::_signal == false)
			throw(std::runtime_error("poll() failed"));
		for (size_t i = 0; i < _fds.size(); i++)
		{
			if (_fds[i].revents & POLLIN)
			{
				// if (_fds[i].fd == _socketFd)
				// 	this->accept_new_client();
				// else
				// 	this->reciveNewData(fds[i].fd);
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
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	int opt = 1;

	if (this->_socketFd == -1)
		throw (std::runtime_error("Failed to create server socket"));

	if (setsockopt(_socketFd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) < 0 ))
		throw (std::runtime_error("Failed to set option (SP_REUSEADDR) on socket"));

	if (fcntl(_socketFd, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("faild to set option (O_NONBLOCK) on socket"));

	if (bind(_socketFd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
		throw (std::runtime_error("Failed to bind"));

	if (listen(_socketFd, SOMAXCONN) == -1)
		throw (std::runtime_error("Failed to listen"));

	newClient.fd = _socketFd;
	newClient.events = POLLIN;
	newClient.revents = 0;
	// fds.push_back(newClient);
}

void	Server::closeFds()
{
	// for (size_t i = 0; i < _clients.size(); i++)
	// {
	// 	std::cout << "Client <" << _clients[i].GetFd() << "> Disconnected" << WHI << std::endl;
	// 	close(_clients[i].GetFd());
	// }
	if (_socketFd != -1){
		std::cout << "Server <" << _socketFd << "> Disconnected" << std::endl;
		close(_socketFd);
	}
}

void Server::removeFds(int fd){
	for (size_t i = 0; i < this->_fds.size(); i++)
	{
		if (this->_fds[i].fd == fd)
			{this->_fds.erase(this->_fds.begin() + i); return;}
	}
}