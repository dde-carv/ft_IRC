#pragma once
#include "Server.hpp"

class Client
{
	private:
		int	_fd;
		std::string	_nickName;
		std::string	_userName;
		bool _operator;
		bool _registered;
		bool _logedIn;

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

		bool	getOperator();
		bool	getRegistered();
		bool	getLogenIn();


		// setters
		void	setFd(int fd);
		void	setNickName(std::string& nickname);
		void	setUserName(std::string& username);

		void	setOperator(bool value);
		void	setRegistered(bool value);
		void	setLogedin(bool value);

};