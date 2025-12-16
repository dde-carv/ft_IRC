#include "Server.hpp"

bool	Server::splitPart(std::vector<std::string> &tokens, \
std::vector<std::string> &channPart, std::string &reason, int fd)
{
	std::string temp;

	if (tokens.empty())
		return false;
	for (size_t i = 0; i < tokens[0].size(); i++)
	{
		if (tokens[0][i] != ',')
			temp += tokens[0][i];
		else
		{
			if (temp.empty())
				continue ;
			channPart.push_back(temp);
			temp.clear();
		}
	}
	for (size_t i = 0; i < channPart.size(); i++)
	{
		if (*(channPart[i].begin()) == '#')
			channPart[i].erase(channPart[i].begin());
		else
		{
			sendResponse(ERR_CHANNELNOTFOUND(this->getClientFd(fd)->getNickName(), channPart[i]), fd);
			channPart.erase(channPart.begin() + i--);
		}
	}
	reason = findMsg(tokens);
	return true;
}

void	Server::part(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string> channPart;
	std::string reason;

	tokens.erase(tokens.begin());
	if (!splitPart(tokens, channPart, reason, fd))
		return (sendResponse(ERR_NOTENOUGHPARAM(this->getClientFd(fd)->getNickName()), fd));
	for (size_t i = 0; i < channPart.size(); i++)
	{
		bool exist = false;
		for (size_t j; j < this->channels.size(); j++)
		{
			if (this->channels[j].getName() == channPart[i])
			{
				exist = true;
				if (!this->channels[j].getClient(fd) && !this->channels[j].getAdmin(fd))
				{
					sendResponse(ERR_NOTONCHANNEL(this->getClientFd(fd)->getNickName(), \
					this->channels[j].getName()), fd);
					continue ;
				}
				std::string msg = ":" + this->getClientFd(fd)->getNickName() + "!~" + \
				this->getClientFd(fd)->getUserName() + "@localhost PART #" + channPart[i];
				if (!reason.empty())
					msg += " :" + reason + CRLF;
				else
					msg += CRLF;
				this->channels[j].sendToAll(msg);
				if(this->channels[j].getAdmin(fd))
					this->channels[j].removeAdmin(fd);
				else
					this->channels[j].removeClient(fd);
				if (this->channels[j].getNumberOfClients == 0)
					this->channels.erase(this->channels.begin() + j);
			}
		}
		if (!exist)
			sendResponse(ERR_CHANNELNOTFOUND(this->getClientFd(fd)->getNickName(), channPart[i]), fd);
	}
}
