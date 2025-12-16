#include "Server.hpp"
//!! carefull with the channel's non-defined arguments in channel construction
//!! ERR_NOTOPERATOR needs the nickname

bool	findRestriction(bool restriction, Client* admin)
{
	if (restriction && !admin)
		return true;
	else
		return false;
}

int	Server::splitTopic(std::vector<std::string> &tokens, std::string &chanName, std::string &topic)
{
	if ((tokens.size() == 1 && tokens[0] == ":") || tokens.size() == 0)
		return 0;
	chanName = tokens[0];
	if (tokens.size() >= 2)
	{
		for (size_t i = 0; i < tokens.size(); i++)
		{
			if (i < tokens.size() - 1)
				topic += tokens[i];
			else
				topic += tokens[i] + " ";
		}
		if (topic[0] == ':')
			topic = topic.substr(1);
	}
	return 1;
}

void	Server::topic(std::vector<std::string> &tokens, int fd)
{
	std::string chanName;
	std::string topic;

	tokens.erase(tokens.begin());
	if (!splitTopic(tokens, chanName, topic))
		return (sendResponse(ERR_NOTENOUGHPARAM(this->getClientFd(fd)->getNickName()), fd));
	if (*(chanName.begin()) == '#')
		chanName.erase(chanName.begin());
	else
		return (sendResponse(ERR_CHANNELNOTFOUND(this->getClientFd(fd)->getNickName(), chanName), fd));
	if (!this->getChannel(chanName)->getClientFd(fd) && !this->getChannel(chanName)->getAdmin(fd))
		return (sendResponse(ERR_NOTONCHANNEL(this->getClientFd(fd)->getNickName(), "#" + chanName), fd));
	if (tokens.size() == 1)
	{
		if (this->getChannel(chanName)->getChannelTopic() == "")
			return (sendResponse(RPL_NOTOPIC(this->getClientFd(fd)->getNickName(), chanName), fd));
		else
			return (sendResponse(RPL_TOPICIS(this->getClientFd(fd)->getNickName(), chanName, this->getChannel(chanName)->getChannelTopic()), fd),\
			sendResponse(RPL_TOPICWHOTIME(this->getClientFd(fd)->getNickName(), chanName, this->getChannel(chanName)->getTopicCreation()), fd));
	}

	bool restriction = findRestriction(this->getChannel(chanName)->getTopicRestriction(), this->getChannel(chanName)->getAdmin(fd));
	if (restriction)
		return (sendResponse(ERR_NOTOPERATOR(chanName), fd));
	if (tokens.size() == 2 && tokens[1] == ":")
	{
		this->getChannel(chanName)->setChannelTopic("");
		this->getChannel(chanName)->setTopicCreation("");
		return (this->getChannel(chanName)->sendToAll(":" + this->getClientFd(fd)->getNickName() + "!" + this->getClientFd(fd)->getUserName() + \
		"localhost TOPIC #" + chanName + " :" + this->getChannel(chanName)->getTopic() + CRLF));
	}
	else if (tokens.size() >= 2 && tokens[1][0] != ':')
	{
		this->getChannel(chanName)->setChannelTopic(tokens[1]);
		this->getChannel(chanName)->setTopicCreation(/*time function*/);
		return (this->getChannel(chanName)->sendToAll(":" + this->getClientFd(fd)->getNickName() + "!" + this->getClientFd(fd)->getUserName() + \
		"localhost TOPIC #" + chanName + " :" + this->getChannel(chanName)->getTopic() + CRLF));
	}
	else
	{
		this->getChannel(chanName)->setChannelTopic(topic);
		this->getChannel(chanName)->setTopicCreation(/*time function*/);
		this->getChannel(chanName)->sendToAll(":" + this->getClientFd(fd)->getNickName() + "!" + this->getClientFd(fd)->getUserName() + \
		"localhost TOPIC #" + chanName + " :" + this->getChannel(chanName)->getTopic() + CRLF);
	}
}
