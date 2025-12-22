#include "../../inc/Server.hpp"


std::string Server::splitQuit(std::vector<std::string> &tokens)
{
	if (tokens.size() == 1)
		return ("Quit");
	else
		return (findMsg(tokens));
}

void	Server::quit(std::vector<std::string> &tokens, int &fd)
{
	std::string reason;

	reason = splitQuit(tokens) + CRLF;
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getClient(fd))
		{
			this->_channels[i].removeClient(fd);
			if (this->_channels[i].getNumberOfClients() == 0)
				removeChannel(this->_channels[i].getName());
			else
				this->_channels[i].sendToAll(":" + this->getClientFd(fd)->getNickName() + \
				"!~" + this->getClientFd(fd)->getUserName() + "@localhost QUIT " + reason);
		}
		if (this->_channels[i].getAdmin(fd))
		{
			this->_channels[i].removeAdmin(fd);
			if (this->_channels[i].getNumberOfClients() == 0)
				removeChannel(this->_channels[i].getName());
			else
				this->_channels[i].sendToAll(":" + this->getClientFd(fd)->getNickName() + \
				"!~" + this->getClientFd(fd)->getUserName() + "@localhost QUIT " + reason);
		}
	}
	this->endConnection(fd);
}
