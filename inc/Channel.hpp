#pragma once

#include "Client.hpp"
#include "Server.hpp"

#define RED "\e[1;31m"
#define WHI "\e[0;37m"
#define GRE "\e[1;32m"
#define YEL "\e[1;33m"

class Client;

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
		std::vector<std::pair<char, bool> > _modes;

	public:

		//*CONSTRUCTORS AND DESTRUCTORS
		Channel();
		Channel(const Channel &og);
		Channel&			operator=(const Channel & og);
		~Channel();

		//*GETTERS
		Client*				getClient(int fd);
		Client*				getAdmin(int fd);
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
