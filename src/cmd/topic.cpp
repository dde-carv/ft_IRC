#include "../../inc/Server.hpp"

//* Syntax: topic/TOPIC #<channel> :topic
//* Ex.: TOPIC #bocal :Space to talk about equipment flaws

//* RPL_NOTOPIC				(331)
//* RPL_TOPICIS				(332)
//* RPL_TOPICWHOTIME		(333)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_NOTONCHANNEL		(442)
//* ERR_NEEDMOREPARAMS		(461)
//* ERR_CHANOPRIVSNEEDED	(482)


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
	if (tokens.size() >= 2 && tokens[1].substr(0, 2) != "::")
		topic = tokens[1].substr(1);
	else
	{
		for (size_t i = 1; i < tokens.size(); i++)
		{
			if (i == tokens.size() - 1)
				topic += tokens[i];
			else
				topic += tokens[i] + " ";
		}
		topic = topic.substr(2);
	}
	return 1;
}

void	Server::topic(std::vector<std::string> &tokens, int &fd)
{
	std::string chanName;
	std::string topic;

	tokens.erase(tokens.begin());
	if (!splitTopic(tokens, chanName, topic))
		return (sendRsp(ERR_NEEDMOREPARAMS(this->getClientFd(fd)->getNickName()), fd));
	if (*(chanName.begin()) == '#')
		chanName.erase(chanName.begin());
	else
		return (sendRsp(ERR_NOSUCHCHANNEL(this->getClientFd(fd)->getNickName(), chanName), fd));
	if (!this->getChannel(chanName)->getClient(fd) && !this->getChannel(chanName)->getAdmin(fd))
		return (sendRsp(ERR_NOTONCHANNEL(this->getClientFd(fd)->getNickName(), "#" + chanName), fd));
	if (tokens.size() == 1)
	{
		if (this->getChannel(chanName)->getChannelTopic() == "")
			return (sendRsp(RPL_NOTOPIC(this->getClientFd(fd)->getNickName(), chanName), fd));
		else
			return (sendRsp(RPL_TOPICIS(this->getClientFd(fd)->getNickName(), chanName, this->getChannel(chanName)->getChannelTopic()), fd),\
			sendRsp(RPL_TOPICWHOTIME(this->getClientFd(fd)->getNickName(), chanName, this->getChannel(chanName)->getTopicCreation()), fd));
	}

	if (findRestriction(this->getChannel(chanName)->getTopicRestriction(), this->getChannel(chanName)->getAdmin(fd)))
		return (sendRsp(ERR_CHANOPRIVSNEEDED(chanName), fd));
	this->getChannel(chanName)->setChannelTopic(topic);
	this->getChannel(chanName)->setTopicCreation(currentTime());
	this->getChannel(chanName)->sendToAll(":" + this->getClientFd(fd)->getNickName() + "!" + this->getClientFd(fd)->getUserName() + \
	"localhost TOPIC #" + chanName + " :" + this->getChannel(chanName)->getChannelTopic() + CRLF);
}
