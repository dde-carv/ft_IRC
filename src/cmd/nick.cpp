#include "../../inc/Server.hpp"

bool Server::validNick(std::string &nickName)
{
	if(!nickName.empty() && (nickName[0] == '&' || nickName[0] == '#' || nickName[0] == ':'))
		return false;
	for(size_t i = 1; i < nickName.size(); i++)
	{
		if(!std::isalnum(nickName[i]) && nickName[i] != '_')
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

void	Server::nick(std::string nickName, int fd)
{
	std::string tmp;
	nickName = nickName.substr(4);
	size_t pos = nickName.find_first_not_of("\t\v ");
	if (pos < nickName.size())
	{
		nickName = nickName.substr(pos);
		if (nickName[0] == ':')
			nickName.erase(nickName.begin());
	}
	Client *cli = getClientFd(fd);
	if (pos == std::string::npos || nickName.empty())
	{
		sendRsp(ERR_NOTENOUGHPARAM(std::string("*")), fd);
		return ;
	}
	if (nickInUse(nickName) && cli->getNickName() != nickName)
	{
		tmp = "*";
		if (cli->getNickName().empty())
			cli->setNickName(tmp);
		sendRsp(ERR_NICKINUSE(nickName), fd);
		return ;
	}
	if (!validNick(nickName))
	{
		sendRsp(ERR_ERRONEUSNICK(nickName), fd);
		return ;
	}
	else
	{
		if (cli && cli->getRegistered())
		{
			std::string oldNick = cli->getNickName();
			cli->setNickName(nickName);
			for (size_t i = 0; i < _channels.size(); i++)
			{
				Client *tmpCl = _channels[i].getClientInChannel(oldNick);
				if (tmpCl)
					tmpCl->setNickName(nickName);
			}
			if (!oldNick.empty() && oldNick != nickName)
			{
				if (oldNick == "*" && !cli->getUserName().empty())
				{
					cli->setLogedin(true);
					sendRsp(RPL_CONNECTED(cli->getNickName()), fd);
					sendRsp(RPL_NICKCHANGE(cli->getNickName(), nickName), fd);
				}
				else
					sendRsp(RPL_NICKCHANGE(oldNick, nickName), fd);
				return ;
			}
		}
		else if (cli && !cli->getRegistered())
			sendRsp(ERR_NOTREGISTERED(nickName), fd);
	}
	if (cli && cli->getRegistered() && !cli->getUserName().empty() && !cli->getNickName().empty() && cli->getNickName() != "*" && !cli->getLogedIn())
	{
		cli->setLogedin(true);
		sendRsp(RPL_CONNECTED(cli->getNickName()), fd);
	}
}
