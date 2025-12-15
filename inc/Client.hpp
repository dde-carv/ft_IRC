#pragma once
#include "Server.hpp"
#include "Channel.hpp"

class Client
{
	private:
		int	_fd;
		std::string _ip;
		std::string	_nickName;
		std::string	_userName;
		bool	_operator;
		bool	_registered;
		bool	_logedIn;
		std::vector<std::string> _channelInviteList;

	public:
		// default constructor
		Client();
		// constructor
		Client(int fd, std::string nickname, std::string username);
		// copy constructor
		Client(Client const &og);
		// copy assignement operator
		Client&	operator=(Client const &og);
		// default destructor
		~Client();

		// getters
		int	getFd();
		std::string	getNickName();
		std::string	getUserName();
		std::string getHostname();
		std::string getIpAddress();
		bool	getOperator();
		bool	getRegistered();
		bool	getLogedIn();
		bool	getInviteToChannel(std::string &channelName);

		// setters
		void	setFd(int fd);
		void	setNickName(std::string& nickname);
		void	setUserName(std::string& username);
		void	setOperator(bool value);
		void	setRegistered(bool value);
		void	setLogedin(bool value);
		void	setIpAdd(std::string ip);

		// methods
		void	AddChannelInvite(std::string &channelname);
		void	removeChannelInvite(std::string &channelName);
};