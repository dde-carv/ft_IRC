#include "../../inc/Server.hpp"

void	Server::pass(std::string password, int fd)
{
	Client *cli = getClientFd(fd);
	password = password.substr(4);
	size_t pos = password.find_first_not_of("\t\v ");
	if (pos < password.size())
	{
		password = password.substr(pos);
		if (password[0] == ':')
			password.erase(password.begin());
	}
	if (pos == std::string::npos || password.empty())
	{
		sendRsp(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return ;
	}
	else if (!cli->getRegistered())
	{
		if (password == _password)
			cli->setRegistered(true);
		else
			sendRsp(ERR_INCORPASS(std::string("*")), fd);
	}
	else
		sendRsp(ERR_ALREADYREGISTERED(cli->getNickName()), fd);
}
