#include "../../inc/Server.hpp"

//* Syntax: part/PART #<channel> :reason
//* Ex.: part #bocal,#cluster1

//* ERR_NOSUCHCHANNEL		(403)
//* ERR_NOTONCHANNEL		(442)
//* ERR_NEEDMOREPARAMS		(461)

void	Server::splitChannelPart(std::vector<std::string> &channPart, std::string &temp)
{
	if (temp.empty())
		return ;
	channPart.push_back(temp);
	temp.clear();
}

bool	Server::splitPart(std::vector<std::string> &tokens, \
std::vector<std::string> &channPart, std::string &reason, int fd)
{
	std::string temp;
	size_t separ = tokens[0].find(',');

	if (tokens.empty())
		return false;
	for (size_t i = 0; i < tokens[0].size(); i++)
	{
		if (separ == std::string::npos)
		{
			temp = tokens[0];
			splitChannelPart(channPart, temp);
			break ;
		}
		if (tokens[0][i] != ',')
			temp += tokens[0][i];
		else
			splitChannelPart(channPart, temp);
	}
	for (size_t i = 0; i < channPart.size(); i++)
	{
		if (*(channPart[i].begin()) == '#')
			channPart[i].erase(channPart[i].begin());
		else
		{
			sendRsp(ERR_NOSUCHCHANNEL(this->getClientFd(fd)->getNickName(), channPart[i]), fd);
			channPart.erase(channPart.begin() + i--);
		}
	}
	reason = findMsg(tokens);
	return true;
}

void	Server::part(std::vector<std::string> &tokens, int &fd)
{
	std::vector<std::string> channPart;
	std::string reason;

	tokens.erase(tokens.begin());
	if (!splitPart(tokens, channPart, reason, fd))
		return (sendRsp(ERR_NEEDMOREPARAMS(this->getClientFd(fd)->getNickName()), fd));
	for (size_t i = 0; i < channPart.size(); i++)
	{
		bool exist = false;
		for (size_t j = 0; j < this->_channels.size(); j++)
		{
			if (this->_channels[j].getName() == channPart[i])
			{
				exist = true;
				if (!this->_channels[j].getClient(fd) && !this->_channels[j].getAdmin(fd))
				{
					sendRsp(ERR_NOTONCHANNEL(this->getClientFd(fd)->getNickName(), \
					this->_channels[j].getName()), fd);
					continue ;
				}
				std::string msg = ":" + this->getClientFd(fd)->getNickName() + "!~" + \
				this->getClientFd(fd)->getUserName() + "@localhost PART #" + channPart[i];
				if (!reason.empty())
					msg += " :" + reason + CRLF;
				else
					msg += " :Leaving\r\n";
				this->_channels[j].sendToAll(msg);
				if(this->_channels[j].getAdmin(fd))
					this->_channels[j].removeAdmin(fd);
				else
					this->_channels[j].removeClient(fd);
				if (this->_channels[j].getNumberOfClients() == 0)
					removeChannel(this->_channels[j].getName());
			}
		}
		if (!exist)
			sendRsp(ERR_NOSUCHCHANNEL(this->getClientFd(fd)->getNickName(), channPart[i]), fd);
	}
}
