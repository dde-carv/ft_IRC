//* Syntax: join/JOIN #<channel> <key>(optional) or join/JOIN #<channel>{,<channel>} <key>{,<key>}(optional)

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_TOOMANYTARGETS		(407)
//* ERR_TOOMANYCHANNELS		(405)
//* ERR_BADCHANNELKEY		(475)
//* ERR_INVITEONLYCHAN		(473)
//* ERR_CHANNELISFULL		(471)

#include "../../inc/Server.hpp"

/* Checks if the client is invited to the channel. */
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
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getClientInChannel(nick))
			count++;
	}
	return count;
}

void	Server::channelNotExist(std::vector<std::pair<std::string, std::string> > token, size_t i, int fd)
{
	if (searchClientInChannels(getClientFd(fd)->getNickName()) >= 10)
		return sendRsp(ERR_TOOMANYCHANNELS(getClientFd(fd)->getNickName()), fd);
	Channel newChannel;
	newChannel.setName(token[i].first);
	newChannel.addAdmin(*getClientFd(fd));
	newChannel.setChannelCreationTime();
	this->_channels.push_back(newChannel);
	sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), newChannel.getName()) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), newChannel.getName(), newChannel.getClientListStr()) + \
			RPL_ENDOFNAMES(getClientFd(fd)->getNickName(), newChannel.getName()), fd);
}

void	Server::channelExist(std::vector<std::pair<std::string, std::string> > token, size_t i, size_t j, int fd)
{
	if (this->_channels[j].getClientInChannel(getClientFd(fd)->getNickName()))
		return ;
	if (searchClientInChannels(getClientFd(fd)->getNickName()) >= 10)
		return sendRsp(ERR_TOOMANYCHANNELS(getClientFd(fd)->getNickName()), fd);
	if (!this->_channels[j].getPassword().empty() && this->_channels[j].getPassword() != token[i].second)
	{
		if (!isInvited(getClientFd(fd), token[i].first, 0))
			return sendRsp(ERR_BADCHANNELKEY(getClientFd(fd)->getNickName(), token[i].first), fd);
	}
	if (this->_channels[j].getInviteOnly())
	{
		if (!isInvited(getClientFd(fd), token[i].first, 1))
			return sendRsp(ERR_INVITEONLYCHAN(getClientFd(fd)->getNickName(), token[i].first), fd);
	}
	if (this->_channels[j].getLimitOfClients() && this->_channels[j].getNumberOfClients() >= this->_channels[j].getLimitOfClients())
		return sendRsp(ERR_CHANNELISFULL(getClientFd(fd)->getNickName(), token[i].first), fd);
	Client *clt = getClientFd(fd);
	this->_channels[j].addClient(*clt);
	if (_channels[j].getChannelTopic().empty())
		sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), token[i].first) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getClientListStr()) + \
			RPL_ENDOFNAMES(getClientFd(fd)->getNickName(), _channels[j].getName()), fd);
	else
		sendRsp(RPL_JOINMSG(getClientFd(fd)->getHostname(), getClientFd(fd)->getIpAddress(), token[i].first) + \
			RPL_TOPICIS(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getChannelTopic()) + \
			RPL_NAMREPLY(getClientFd(fd)->getNickName(), _channels[j].getName(), _channels[j].getClientListStr()) + \
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
			sendRsp(ERR_NOSUCHCHANNEL(getClientFd(fd)->getNickName(), token[i].first), fd);
			token.erase(token.begin() + i--);
		}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

void	Server::join(std::vector<std::string> &cmd, int &fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (!splitJoin(token, cmd, fd))
		return sendRsp(ERR_NEEDMOREPARAMS(getClientFd(fd)->getNickName()), fd);
	if (token.size() > 10)
		return sendRsp(ERR_TOOMANYTARGETS(getClientFd(fd)->getNickName()), fd);
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->_channels.size(); j++)
		{
			if (this->_channels[j].getName() == token[i].first)
			{
				channelExist(token, i, j, fd);
				flag = true;
				break ;
			}
		}
		if (!flag)
			channelNotExist(token, i, fd);
	}
}
