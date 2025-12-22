//* Syntax: user/USER <username> 0 * <realname>

//* ERR_NEEDMOREPARAMS (461)
//* ERR_NOTREGISTERED (451)
//* ERR_ALREADYREGISTERED (462)
//* RPL_CONNECTED (001)

#include "../../inc/Server.hpp"

void	Server::user(std::vector<std::string> &cmd, int &fd)
{
	Client *client = getClientFd(fd);

	// std::cout << "is registered? " <<client->getRegistered() << std::endl;

	if (client && cmd.size() < 5)
		return sendRsp(ERR_NEEDMOREPARAMS(client->getNickName()), fd);
	if (!client || !client->getRegistered() || cmd[2] != "0" || cmd [3] != "*")
		sendRsp(ERR_NOTREGISTERED(std::string("*")), fd);
	else if (client && !client->getUserName().empty())
		return sendRsp(ERR_ALREADYREGISTERED(client->getNickName()), fd);
	else
		client->setUserName(cmd[1]);
	if (client && client->getRegistered() && !client->getUserName().empty() && !client->getNickName().empty() && client->getNickName() != "*"  && !client->getLogedIn())
	{
		client->setLogedin(true);
		sendRsp(RPL_CONNECTED(client->getNickName()), fd);
	}
}
