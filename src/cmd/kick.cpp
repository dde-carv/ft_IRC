#include "../../inc/Server.hpp"

void	Server::kick(std::vector<std::string> &cmd, int &fd)
{
	if (cmd.size() < 3)
	{
		senderr(461, getClientFd(fd)->getNickName(), fd, ": Not enough parameters!\r\n");
		return ;
	}

	std::string channels = cmd[1];
	std::string user = cmd[2];
	std::string reason;

	if (cmd.size() > 3)
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
			senderr(403, getClientFd(fd)->getNickName(), currentChannel, fd, ": No such channel!\r\n");
			continue ;
		}
		currentChannel.erase(0, 1);

		Channel *channel = getChannel(currentChannel);
		if (!channel)
		{
			senderr(403, getClientFd(fd)->getNickName(), "#" + currentChannel, fd, ": No such channel!\r\n");
			continue ;
		}
		if (!channel->getClient(fd) && !channel->getAdmin(fd))
		{
			senderr(442, getClientFd(fd)->getNickName(), "#" + currentChannel, fd,  ": You are not in that channel!\r\n");
			continue ;
		}
		if (!channel->getAdmin(fd))
		{
			senderr(482, getClientFd(fd)->getNickName(), "#" + currentChannel, fd,  ": You are not a channel operator!\r\n");
			continue ;
		}

		Client *target = channel->getClientInChannel(user);
		if (!target)
		{
			senderr(441, getClientFd(fd)->getNickName(), "#" + currentChannel, fd,  ": They are not in that channel!\r\n");
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
