#include "../../inc/Server.hpp"

//* Syntax: join/JOIN #<channel> <key>(optional) or join/JOIN #<channel>{,<channel>} <key>{,<key>}(optional)
//* Ex.: JOIN #lol password | join #chat,#development password | join #chat,#development password,password2

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_TOOMANYTARGETS		(407)
//* ERR_TOOMANYCHANNELS		(405)

int		Server::searchClientInChannels(std::string nick)
{
	int count = 0;
	for (int i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getClientInChannel(nick))
			count++;
	}
	return count;
}

// channelNotExist

void	Server::channelExist(std::vector<std::pair<std::string, std::string> > token, size_t i, size_t j, int fd)
{
	if (this->_channels[j].getClientInChannel(getClientFd(fd)->getNickName()))
		return ;
	if (searchClientInChannels(getClientFd(fd)->getNickName()) >= 10)
	{
		senderr(405, getClientFd(fd)->getNickName(), fd, ": You have joined too many channels!\r\n");
		return ;
	}
	if (!this->_channels[j].getPassword().empty() && this->_channels[j].getPassword() != token[i].second)
	{}
}

int		Server::splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
{
	std::vector<std::string>	tmp;
	std::string					chStr, passStr, buff;
	std::istringstream			iss(cmd);
	while(iss >> cmd)
		tmp.push_back(cmd);
	if (tmp.size() < 2)
	{
		token.clear();
		return 0;
	}
	tmp.erase(tmp.begin());
	chStr = tmp[0];
	tmp.erase(tmp.begin());
	if (!tmp.empty())
	{
		passStr = tmp[0];
		tmp.clear();
	}
	for (size_t i = 0; i < chStr.size(); i++)
	{
		if (chStr[i] == ',')
		{
			token.push_back(std::make_pair(buff, ""));
			buff.clear();
		}
		else
			buff += chStr[i];
	}
	token.push_back(std::make_pair(buff, ""));
	if (!passStr.empty())
	{
		size_t j = 0;
		buff.clear();
		for (size_t i = 0; i < passStr.size(); i++)
		{
			if (passStr[i] == ',')
			{
				token[j].second = buff;
				j++;
				buff.clear();
			}
			else
				buff += passStr[i];
		}
		token[j].second = buff;
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		if (token[i].first.empty())
			token.erase(token.begin() + i--);
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		if (*(token[i].first.begin()) != '#')
		{
			senderr(403, getClientFd(fd)->getNickName(), token[i].first, fd, ": There is no such channel!\r\n");
			token.erase(token.begin() + i--);
		}
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

void	Server::join(std::string cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (!splitJoin(token, cmd, fd))
	{
		senderr(461, getClientFd(fd)->getNickName(), fd, ": Not enough parameters!\r\n");
		return ;
	}
	if (token.size() > 10)
	{
		senderr(407, getClientFd(fd)->getNickName(), fd, ": Too many channels!\r\n");
		return ;
	}
	for (size_t i = 0; i < token.size(); i++)
	{
		bool flag = false;
		for (size_t j = 0; j < this->_channels.size(); j++)
		{
			if (this->_channels[j].getName() == token[i].first)
			{
				channelExist(token, i, j, fd);
				flag = true;
				break;
			}
		}
		if (!flag)
			channelNotExist(token, i, fd);
	}
}
