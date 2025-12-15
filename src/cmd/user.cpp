#include "../../inc/Server.hpp"

void	Server::user(std::vector<std::string> &cmd, int fd)
{
	Client *client = getClientFd(fd);
	if (client && cmd.size() < 5)
	{
		sendRsp(ERR_NOTENOUGHPARAM(client->getNickName()), fd);
		return ;
	}
	if (!client || !client->getRegistered())
		sendRsp(ERR_NOTREGISTERED(std::string("*")), fd);
	else if (client && !client->getUserName().empty())
	{
		sendRsp(ERR_ALREADYREGISTERED(client->getNickName()), fd);
		return ;
	}
	else
		client->setUserName(cmd[1]);
	if (client && client->getRegistered() && !client->getUserName().empty() && !client->getNickName().empty() && client->getNickName() != "*"  && !client->getLogedIn())
	{
		client->setLogedin(true);
		sendRsp(RPL_CONNECTED(client->getNickName()), fd);
	}
}
