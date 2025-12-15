
#include "../inc/Channel.hpp"

//*CONSTRUCTORS AND DESTRUCTORS

Channel::Channel()
: _channelName(), _channelCreation(/* funcao geral de definicao de tempo */), _clients(), _admins(), _inviteOnly(false), _key(false), \
_password(), _channelTopic(), _topicRestriction(false), _topicCreation(), _limitOfClients(0), _modes()
{}

Channel::Channel(const Channel &og)
{
	*this = og;
}

Channel& Channel::operator=(const Channel &og)
{
	if (this != &og)
	{
		_channelName = og._channelName;
		_channelCreation = og._channelCreation;
		_clients = og._clients;
		_admins = og._admins;
		_inviteOnly = og._inviteOnly;
		_key = og._key;
		_password = og._password;
		_channelTopic = og._channelTopic;
		_topicRestriction = og._topicRestriction;
		_topicCreation = og._topicCreation;
		_limitOfClients = og._limitOfClients;
		_modes = og._modes;
	}
	return *this;
}

Channel::~Channel()
{}


//*GETTERS

Client*	Channel::getClient(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client*	Channel::getAdmin(int fd)
{
	for (std::vector<Client>::iterator it = _admins.begin(); it != _admins.end(); ++it)
	{
		if (it->getFd() == fd)
			return &(*it);
	}
	return NULL;
}

Client*	Channel::getClientInChannel(std::string name)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getNickName() == name)
			return &(*it);
	}
	for (std::vector<Client>::iterator it = _admins.begin(); it != _admins.end(); ++it)
	{
		if (it->getNickName() == name)
			return &(*it);
	}
	return NULL;
}

std::string Channel::getName() const
{
	return _channelName;
}

std::string Channel::getChannelCreation() const
{
	return _channelCreation;
}

std::string Channel::getPassword() const
{
	return _password;
}

std::string Channel::getChannelTopic() const
{
	return _channelTopic;
}

std::string Channel::getTopicCreation() const
{
	return _topicCreation;
}

std::string Channel::getModes()
{
	std::string mode;
	for (size_t i = 0; i < _modes.size(); i++)
	{
		if (_modes[i].first != 'o' && _modes[i].second)
			mode.push_back(_modes[i].first);
	}
	if (!mode.empty())
		mode.insert(mode.begin(), '+');
	return mode;
}

std::vector<Client> Channel::getClientList() const
{
	return _clients;
}

std::vector<Client> Channel::getAdminList() const
{
	return _admins;
}

bool Channel::getInviteOnly() const
{
	return _inviteOnly;
}

bool Channel::getKey() const
{
	return _key;
}

bool Channel::getTopicRestriction() const
{
	return _topicRestriction;
}

bool Channel::getModeAtIndex(size_t i)
{
	return _modes[i].second;
}

unsigned int Channel::getNumberOfClients() const
{
	return this->_clients.size() + this->_admins.size();
}

unsigned int Channel::getLimitOfClients() const
{
	return _limitOfClients;
}


//*SETTERS

void Channel::setName(std::string name)
{
	_channelName = name;
}

void Channel::setChannelCreationTime()
{
	std::time_t _time = std::time(NULL);
	std::ostringstream oss;
	oss << _time;
	this->_channelCreation = std::string(oss.str());
}

void Channel::setPassword(std::string pass)
{
	_password = pass;
}

void Channel::setChannelTopic(std::string topic)
{
	_channelTopic = topic;
}

void Channel::setTopicCreation(std::string date)
{
	_topicCreation = date;
}

void Channel::setClientList(std::vector<Client> cList)
{
	_clients = cList;
}

void Channel::setAdminList(std::vector<Client> aList)
{
	_admins = aList;
}

void Channel::setInviteOnly(bool invite)
{
	_inviteOnly = invite;
}

void Channel::setKey(bool key)
{
	_key = key;
}

void Channel::setTopicRestriction(bool tRestriction)
{
	_topicRestriction = tRestriction;
}

void Channel::setLimitOfClients(unsigned int cLimit)
{
	_limitOfClients = cLimit;
}

void Channel::setModeAtIndex(size_t i, bool mode)
{
	_modes[i].second = mode;
}

//*METHODS

void	Channel::addClient(Client newClient)
{
	_clients.push_back(newClient);
}

void	Channel::addAdmin(Client newClient)
{
	_admins.push_back(newClient);
}

void	Channel::removeClient(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			_clients.erase(it);
			break ;
		}
	}
}

void	Channel::removeAdmin(int fd)
{
	for (std::vector<Client>::iterator it = _admins.begin(); it != _admins.end(); ++it)
	{
		if (it->getFd() == fd)
		{
			_admins.erase(it);
			break ;
		}
	}
}

bool	Channel::clientToAdmin(std::string &nick)
{
	size_t i = 0;
	for(; i < _clients.size(); ++i)
	{
		if (_clients[i].getNickName() == nick)
			break;
	}
	if (i < _clients.size())
	{
		_admins.push_back(_clients[i]);
		_clients.erase(i + _clients.begin());
		return true;
	}
	return false;
}

bool	Channel::adminToClient(std::string &nick)
{
	size_t i = 0;
	for(; i < _admins.size(); ++i)
	{
		if (_admins[i].getNickName() == nick)
			break;
	}
	if (i < _admins.size())
	{
		_clients.push_back(_admins[i]);
		_admins.erase(i + _admins.begin());
		return true;
	}
	return false;
}

void	Channel::sendToAll(std::string repl)
{
	for (size_t i = 0; i < _admins.size(); ++i)
		if (send(_admins[i].getFd(), repl.c_str(), repl.size(), 0) == -1)
			std::cout << "send() failed" << std::endl;
	for (size_t i = 0; i < _clients.size(); ++i)
		if (send(_clients[i].getFd(), repl.c_str(), repl.size(), 0) == -1)
			std::cout << "send() failed" << std::endl;
}

void	Channel::sendToEveryoneElse(std::string repl, int fd)
{
	for (size_t i = 0; i < _admins.size(); ++i)
	{
		if (_admins[i].getFd() != fd)
			if (send(_admins[i].getFd(), repl.c_str(), repl.size(), 0) == -1)
				std::cout << "send() failed" << std::endl;
	}
	for (size_t i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i].getFd() != fd)
			if (send(_clients[i].getFd(), repl.c_str(), repl.size(), 0) == -1)
				std::cout << "send() failed" << std::endl;
	}
}
