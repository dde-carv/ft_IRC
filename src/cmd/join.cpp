#include "../../inc/Server.hpp"

//* Syntax: join/JOIN #<channel> <key>(optional) or join/JOIN #<channel>{,<channel>} <key>{,<key>}(optional)
//* Ex.: JOIN #lol password | join #chat,#development password | join #chat,#development password,password2

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_TOOMANYTARGETS		(407)
//* ERR_TOOMANYCHANNELS		(405)
//* ERR_BADCHANNELKEY		(475)
//* ERR_INVITEONLYCHAN		(473)
//* ERR_CHANNELISFULL		(471)

bool	isInvited(Client *clt, std::string channelName, int flag)
{
	if (clt->getInviteToChannel(channelName))
	{
		if (flag == 1)
			clt->removeChannelInvite(channelName);
		return true;
	}
	return false;
}

int		Server::searchClientInChannels(std::string nick)
{
	int count = 0;
	for (int i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getClientInChannel(nick))
			count++;
	}
	return count;
}

void	Server::channelNotExist(std::vector<std::pair<std::string, std::string> > token, size_t i, int fd)
{
	if (searchClientInChannels(getClientFd(fd)->getNickName()) >= 10)
	{
		senderr(405, getClientFd(fd)->getNickName(), fd, ": You have joined too many channels!\r\n");
		return ;
	}
	Channel newChannel;
	newChannel.setName(token[i].first);
	newChannel.addAdmin(*getClientFd(fd));
	newChannel.setChannelCreationTime();
	this->_channels.push_back(newChannel);
	sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), newChannel.getName()) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), newChannel.getName(), newChannel.getClientList()) + \
			RPL_ENDOFNAMES(getClientFd(fd)->getNickName(), newChannel.getName()), fd);
}

void	Server::channelExist(std::vector<std::pair<std::string, std::string> > token, size_t i, size_t j, int fd)
{
	if (this->_channels[j].getClientInChannel(getClientFd(fd)->getNickName()))
		return ;
	if (searchClientInChannels(getClientFd(fd)->getNickName()) >= 10)
	{
		senderr(405, getClientFd(fd)->getNickName(), fd, ": You have joined too many channels!\r\n");
		return ;
	}
	if (!this->_channels[j].getPassword().empty() && this->_channels[j].getPassword() != token[i].second)
	{
		if (!isInvited(getClientFd(fd), token[i].first, 0))
		{
			senderr(475, getClientFd(fd)->getNickName(), "#" + token[i].first, fd, ": Cannot join the channel - bad password!\r\n");
			return ;
		}
	}
	if (this->_channels[j].getInviteOnly())
	{
		if (!isInvited(getClientFd(fd), getClientFd(fd)->getNickName(), 1))
		{
			senderr(473, getClientFd(fd)->getNickName(), "#" + token[i].first, fd, ": Cannot join the channel - invite only!\r\n");
			return ;
		}
	}
	if (this->_channels[j].getLimitOfClients() && this->_channels[j].getNumberOfClients() >= this->_channels[j].getLimitOfClients())
	{
		senderr(471, getClientFd(fd)->getNickName(), "#" + token[i].first, fd, ": Cannot join the channel - is full!\r\n");
		return ;
	}
	Client *clt = getClientFd(fd);
	this->_channels[j].addClient(*clt);
	if (_channels[j].getChannelTopic().empty())
		sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), token[i].first) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getClientList()) + \
			RPL_ENDOFNAMES(getClientFd(fd)->getNickName(), _channels[j].getName()), fd);
	else
		sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), token[i].first) + \
			RPL_TOPICIS(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getChannelTopic()) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getClientList()) + \
			RPL_ENDOFNAMES(getClientFd(fd)->getNickName(), _channels[j].getName()), fd);
	_channels[j].sendToEveryoneElse(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), token[i].first), fd);
}

int		Server::splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::vector<std::string> &cmd, int fd)
{
	std::vector<std::string>	tmp = cmd;
	std::string					chStr, passStr, buff;

	if (tmp.size() < 2)
	{
		token.clear();
		return 0;
	}
	tmp.erase(tmp.begin());
	chStr = tmp[0];
	tmp.erase(tmp.begin());
	if (!tmp.empty())
	{
		passStr = tmp[0];
		tmp.clear();
	}
	for (size_t i = 0; i < chStr.size(); i++)
	{
		if (chStr[i] == ',')
		{
			token.push_back(std::make_pair(buff, ""));
			buff.clear();
		}
		else
			buff += chStr[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!passStr.empty())
	{
		size_t j = 0;
		buff.clear();
		for (size_t i = 0; i < passStr.size(); i++)
		{
			if (passStr[i] == ',')
			{
				token[j].second = buff;
				j++;
				buff.clear();
			}
			else
				buff += passStr[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		if (token[i].first.empty())
			token.erase(token.begin() + i--);
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		if (*(token[i].first.begin()) != '#')
		{
			senderr(403, getClientFd(fd)->getNickName(), token[i].first, fd, ": There is no such channel!\r\n");
			token.erase(token.begin() + i--);
		}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

void	Server::join(std::vector<std::string> &cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (!splitJoin(token, cmd, fd))
	{
		senderr(461, getClientFd(fd)->getNickName(), fd, ": Not enough parameters!\r\n");
		return ;
	}
	if (token.size() > 10)
	{
		senderr(407, getClientFd(fd)->getNickName(), fd, ": Too many channels!\r\n");
		return ;
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->_channels.size(); j++)
		{
			if (this->_channels[j].getName() == token[i].first)
			{
				channelExist(token, i, j, fd);
				flag = true;
				break;
			}
		}
		if (!flag)
			channelNotExist(token, i, fd);
	}
}
