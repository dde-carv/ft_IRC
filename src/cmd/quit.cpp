#include "Server.hpp"


std::string SplitQuit(std::vector<std::string> &tokens)
{
	if (tokens.size() == 1)
		return ("Quit");
	else
		return (findMsg(tokens));
}

void	Server::quit(std::vector<std::string> &tokens, int fd)
{
	std::string reason;

	reason = splitQuit(tokens) + "\r\n";
	for (size_t i = 0; i < this->channels.size(); i++)
	{
		if (this->channels[i]->getClient(fd))
		{
			this->channels[i]->removeClient(fd);
			if (this->channels[i]->getNumberOfClients == 0)
				this->channels.erase(this->channels.begin() + i);
			else
				this->channels[i].sendToAll(":" + this->getClient(fd)->getNickName() + \
				"!~" + this->getClient(fd)->getUserName() + "@localhost QUIT " + reason);
		}
		if (this->channels[i]->getAdmin(fd))
		{
			this->channels[i]->removeAdmin(fd);
			if (this->channels[i]->getNumberOfClients == 0)
				this->channels.erase(this->channels.begin() + i);
			else
				this->channels[i].sendToAll(":" + this->getClient(fd)->getNickName() + \
				"!~" + this->getClient(fd)->getUserName() + "@localhost QUIT " + reason);
		}
	}
	this->endConnection(fd);
}
