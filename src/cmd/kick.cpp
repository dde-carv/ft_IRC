//* Syntax: kick/KICK #<channel>{,<channel>} <user> <reason>(optional)

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_NOTONCHANNEL		(442)
//* ERR_CHANOPRIVSNEEDED	(482)
//* ERR_USERNOTINCHANNEL	(441)

#include "../../inc/Server.hpp"

void	Server::kick(std::vector<std::string> &cmd, int &fd)
{
	if (cmd.size() < 3)
		return sendRsp(ERR_NEEDMOREPARAMS(getClientFd(fd)->getNickName()), fd);

	std::string channels = cmd[1];
	std::string user = cmd[2];
	std::string reason;

	if (cmd.size() > 3 && cmd[3][0] == ':')
	{
		for (size_t i = 3; i < cmd.size(); i++)
		{
			reason += cmd[i];
			if (i < cmd.size() - 1)
				reason += " ";
		}
		if (!reason.empty() && reason[0] == ':')
			reason.erase(0, 1);
	}
	else
	{
		reason = cmd[3];
		if (!reason.empty() && reason[0] == ':')
			reason.erase(0, 1);
	}

	std::vector<std::string> tmp;
	std::stringstream ss(channels);
	std::string segment;

	while (std::getline(ss, segment, ','))
	{
		if (!segment.empty())
			tmp.push_back(segment);
	}

	for (size_t i = 0; i < tmp.size(); i++)
	{
		std::string currentChannel = tmp[i];

		if (currentChannel.empty() || currentChannel[0] != '#')
		{
			sendRsp(ERR_NOSUCHCHANNEL(getClientFd(fd)->getNickName(), currentChannel), fd);
			continue ;
		}
		currentChannel.erase(0, 1);

		Channel *channel = getChannel(currentChannel);
		if (!channel)
		{
			sendRsp(ERR_NOSUCHCHANNEL(getClientFd(fd)->getNickName(), currentChannel), fd);
			continue ;
		}
		if (!channel->getClient(fd) && !channel->getAdmin(fd))
		{
			sendRsp(ERR_NOTONCHANNEL(getClientFd(fd)->getNickName(), currentChannel), fd);
			continue ;
		}
		if (!channel->getAdmin(fd))
		{
			sendRsp(ERR_CHANOPRIVSNEEDED(currentChannel), fd);
			continue ;
		}

		Client *target = channel->getClientInChannel(user);
		if (!target)
		{
			sendRsp(ERR_USERNOTINCHANNEL(getClientFd(fd)->getNickName(), user, currentChannel), fd);
			continue ;
		}

		std::stringstream msg;
		msg << ":" << getClientFd(fd)->getNickName() << "!~" << getClientFd(fd)->getUserName() << "@localhost KICK #" << currentChannel << " " << user;
		if (!reason.empty())
			msg << " :" << reason << CRLF;
		else
			msg << CRLF;

		channel->sendToAll(msg.str());

		if (channel->getAdmin(target->getFd()))
			channel->removeAdmin(target->getFd());
		else
			channel->removeClient(target->getFd());

		if (channel->getNumberOfClients() == 0)
			removeChannel(currentChannel);
	}
}
