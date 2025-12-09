#include "../../inc/Server.hpp"

//* Syntax: join/JOIN #<channel> <key>(optional) or join/JOIN #<channel>{,<channel>} <key>{,<key>}(optional)
//* Ex.: JOIN #lol password | join #chat,#development password | join #chat,#development password,password2

//* ERR_NEEDMOREPARAMS		(461)

//!! ADD "join" TO SERVER CLASS (Server::)
//!! ADD "senderr", "getClient", "getChannel", "getClientNick" TO SERVER CLASS

int	splitJoin(std::vector<std::pair<std::string, std::string> > &token, std::string cmd, int fd)
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
		for (size_t i = 0; i < passStr.size(), i++)
		{
			if (passStr[i] == ',')
			{
				token[j].second = buff;
				j++;
				buff.clear();
			}
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
			senderr(403, getClient(fd)->getNick(), token[i].first, fd, ": There is no such channel!\r\n");
		else
			token[i].first.erase(token[i].first.begin());
	}
	return 1;
}

void	join(std::string cmd, int fd)
{
	std::vector<std::pair<std::string, std::string> > token;
	if (!splitJoin(token, cmd, fd))
	{
		senderr(461, getClient(fd)->getNick(), fd, ": Not enough parameters!\r\n");
		return ;
	}
	//!? ...
}
