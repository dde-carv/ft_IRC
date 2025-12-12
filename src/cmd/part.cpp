#include "Server.hpp"

std::string	findMsg(std::vector<std::string> &tokens)
{
	std::string temp = "";

	if (tokens.size() == 1)
		return temp;
	if (tokens[1][0] == ':')
	{
		tokens[1].erase(tokens[1].begin());
		for (int i = 1; i < tokens.size(); i++)
		{
			if (tokens.size() == i + 1)
				temp += tokens[i];
			else
				temp += tokens[i] + " ";
		}
	}
	else
		temp = tokens[1];
	return temp;
}

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
			sendResponse(ERR_CHANNELNOTFOUND(this->getClientFd(fd)->getNickName()), channPart[i]);
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
	for (size_t i = 0; i < chanPart.size(); i++)
	{
		bool exist = false;
		for (size_t j; j < this->channels.size(); j++)
		{
			if (this->channels[j].getName() == chanPart[i])
			{
				exist = true;
				if (!this->channels[j].getClient(fd) && !this->channels[j].getAdmin(fd))
				{
					sendResponse(EERR_NOTONCHANNEL(this->getClient(fd)->getNickName(), \
					this->channels[j].getName()), fd);
					continue ;
				}
				std::string msg = ":" + this->getClient(fd)->getNickName() + "!~" + \
				this->getClient(fd)->getUserName() + "@localhost PART #" + chanPart[i];
				if (!reason.empty())
					msg += " :" + reason + "\r\n";
				else
					msg += "\r\n";
				this->channels[j].sendToAll(msg);
				if(this->channels[j].getAdmin(fd))
					this->channels[j].removeAdmin(fd);
				else
					this->channels[j].removeClient(fd);
				if (this->channels[j].getNumberOfClients == 0)
					this->channels[j].erase(this->channels[j].begin() + j);
			}
		}
		if (!exist)
			sendResponse(ERR_CHANNELNOTFOUND(this->getClient(fd)->getNickName(), chanPart[i]), fd);
	}
}
