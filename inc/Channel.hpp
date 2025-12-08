#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
// #include "Client.hpp"
// #include "Channel.hpp"
// #include "replies.hpp"

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"


class Channel
{
	private:
		std::string			_channelName;
		std::string			_channelCreation;
		std::vector<Client>	_clients;
		std::vector<Client>	_admins;
		bool				_inviteOnly;
		bool				_key;
		std::string			_password;
		std::string			_channelTopic;
		bool				_topicRestriction;
		std::string			_topicCreation;
		unsigned int		_limitOfClients;
		std::vector<std::pair<char, bool>> _modes;

	public:

		//*CONSTRUCTORS AND DESTRUCTORS
		Channel();
		Channel(const Channel &og);
		Channel&			operator=(const Channel & og);
		~Channel();

		//*GETTERS
		Client*				getClient(int fd) const;
		Client*				getAdmin(int fd) const;
		std::string 		getName() const;
		std::string			getChannelCreation() const;
		std::string			getPassword() const;
		std::string			getChannelTopic() const;
		std::string			getTopicCreation() const;
		std::vector<Client>	getClientList() const;
		std::vector<Client>	getAdminList() const;
		bool				getInviteOnly() const;
		bool				getKey() const;
		bool				getTopicRestriction()const;
		unsigned int		getLimitOfClients() const;

		//*SETTERS
		void				setName(std::string);
		void				setChannelCreation(std::string);
		void				setPassword(std::string);
		void				setChannelTopic(std::string);
		void				setTopicCreation(std::string);
		void				setClientList(std::vector<Client>);
		void				setAdminList(std::vector<Client>);
		void				setInviteOnly(bool);
		void				setKey(bool);
		void				setTopicRestriction(bool);
		void				setLimitOfClients(unsigned int);

		//*METHODS
		void				addClient(Client newClient);
		void				addAdmin(Client newClient);
		void				removeClient(int fd);
		void				removeAdmin(int fd);
		bool				clientToAdmin(std::string &nick);
		bool				adminToClient(std::string &nick);
		void				sendToAll(std::string repl);
		void				sendToEveryoneElse(std::string repl, int fd);
};

#endif
