#include "Client.hpp"

Client::Client() : _fd(-1), _operator(false), _registered(false), _logedIn(false)
{}

Client::Client(int fd, std::string nickname, std::string username)
{
	fd = _fd;
	nickname = _nickName;
	username = _userName;
}

Client::Client(Client const &og)
{
	*this = og;
}

Client&	Client::operator=(Client const &og)
{
	if (this != &og)
	{
		this->_fd = og._fd;
		this->_nickName = og._nickName;
		this->_userName = og._userName;
	}
	return *this;
}

Client::~Client()
{}

// getters

int	Client::getFd()
{
	return this->_fd;
}

std::string	Client::getNickName()
{
	return this->_nickName;
}

std::string	Client::getUserName()
{
	return this->_userName;
}

std::string	Client::getHostname()
{
	std::string hostname = this->getNickName() + "!" + this->getUserName();
	return hostname;
}

std::string	Client::getIpAddress()
{
	return this->_ip;
}

bool	Client::getOperator()
{
	return this->_operator;
}

bool	Client::getRegistered()
{
	return this->_registered;
}

bool	Client::getLogedIn()
{
	return this->_logedIn;
}

bool	Client::getInviteToChannel(std::string &channelName)
{
	for (size_t i = 0; i < this->_channelInviteList.size(); i++)
	{
		if (this->_channelInviteList[i] == channelName)
			return true;
	}
	return false;
}

// setters

void	Client::setFd(int fd)
{
	this->_fd = fd;
}

void	Client::setNickName(std::string& nickname)
{
	this->_nickName = nickname;
}

void	Client::setUserName(std::string& username)
{
	this->_userName = username;
}

void	Client::setOperator(bool value)
{
	this->_operator = value;
}

void	Client::setRegistered(bool value)
{
	this->_registered = value;
}

void	Client::setLogedin(bool value)
{
	this->_logedIn = value;
}

void Client::setIpAdd(std::string ip)
{
	this->_ip = ip;
}

void	Client::AddChannelInvite(std::string &channelname)
{
	_channelInviteList.push_back(channelname);
}

void	Client::removeChannelInvite(std::string &channelName)
{
	for (size_t i = 0; i < this->_channelInviteList.size(); i++)
	{
		if (this->_channelInviteList[i] == channelName)
		{
			this->_channelInviteList.erase(this->_channelInviteList.begin() + i);
			return ;
		}
	}
}
