//* Syntax: nick/NICK <nickname>

//* ERR_NONICKNAMEGIVEN (431)
//* ERR_ERRONEUSNICKNAME (432)
//* ERR_NICKNAMEINUSE (433)
//* ERR_NOTREGISTERED (451)

#include "../../inc/Server.hpp"

bool Server::validNick(std::string &nickName)
{
	if (nickName.size() > 30)
		return false;
	if(!nickName.empty() && !std::isalpha(nickName[0]))
		return false;
	for(size_t i = 1; i < nickName.size(); i++)
	{
		if(!std::isalnum(nickName[i]) && nickName[i] != '_' && nickName[i] != '[' && \
			nickName[i] != ']' && nickName[i] != '{' && nickName[i] != '}' && nickName[i] != '\\' && \
			nickName[i] != '|')
			return false;
	}
	return true;
}

bool	Server::nickInUse(std::string &nickName)
{
	for (size_t i = 0; i < this->_clients.size(); i++)
	{
		if (this->_clients[i].getNickName() == nickName)
			return true;
	}
	return false;
}

void	Server::nick(std::vector<std::string> &cmd, int &fd)
{
	if (cmd.size() != 2)
		return sendRsp(ERR_NONICKNAMEGIVEN(std::string("*")), fd);
	std::string tmp;
	Client *cli = getClientFd(fd);
	if (nickInUse(cmd[1]) && cli->getNickName() != cmd[1])
	{
		tmp = "*";
		if (cli->getNickName().empty())
			cli->setNickName(tmp);
		return sendRsp(ERR_NICKNAMEINUSE(cmd[1]), fd);
	}
	if (!validNick(cmd[1]))
		return sendRsp(ERR_ERRONEUSNICKNAME(cmd[1]), fd);
	else
	{
		//std::cout << "nickname: " << cmd[1] << std::endl;
		if (cli && cli->getRegistered())
		{
			std::string oldNick = cli->getNickName();
			cli->setNickName(cmd[1]);
			for (size_t i = 0; i < _channels.size(); i++)
			{
				Client *tmpCl = _channels[i].getClientInChannel(oldNick);
				if (tmpCl)
					tmpCl->setNickName(cmd[1]);
			}
			if (!oldNick.empty() && oldNick != cmd[1])
			{
				if (oldNick == "*" && !cli->getUserName().empty())
				{
					cli->setLogedin(true);
					sendRsp(RPL_CONNECTED(cli->getNickName()), fd);
					sendRsp(RPL_NICKCHANGE(cli->getNickName(), cmd[1]), fd);
				}
				else
					sendRsp(RPL_NICKCHANGE(oldNick, cmd[1]), fd);
				return ;
			}
		}
		else if (cli && !cli->getRegistered())
			sendRsp(ERR_NOTREGISTERED(cmd[1]), fd);
	}
	if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty() && cli->getNickName() != "*" && !cli->getLogedIn())
	{
		cli->setLogedin(true);
		sendRsp(RPL_CONNECTED(cli->getNickName()), fd);
	}
}
