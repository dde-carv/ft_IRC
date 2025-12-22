//* Syntax: pass/PASS <password>

//* ERR_NEEDMOREPARAMS (461)
//* ERR_INCORPASS (464)
//* ERR_ALREADYREGISTERED (462)

#include "../../inc/Server.hpp"

void	Server::pass(std::vector<std::string> &cmd, int &fd)
{
	Client *cli = getClientFd(fd);
	if (cmd.size() != 2)
		return sendRsp(ERR_NEEDMOREPARAMS(std::string("*")), fd);
	else if (!cli->getRegistered())
	{
		//std::cout << "password: " << cmd[1] << std::endl;
		if (cmd[1] == _password)
			cli->setRegistered(true);
		else
			sendRsp(ERR_INCORPASS(std::string("*")), fd);
	}
	else
		sendRsp(ERR_ALREADYREGISTERED(cli->getNickName()), fd);
}
