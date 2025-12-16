#include "Server.hpp"

std::string currentTime()
{
	std::time_t current = std::time(NULL);
	std::stringstream res;

	res << current;
	return res.str();
}

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


void	Server::parseMessage(std::string &cmd, int &fd)
{
	std::vector<std::string> tokens;
	std::stringstream ss(cmd);
	std::string tmp;

	while(ss >> tmp)
	{
		tokens.push_back(tmp);
		tmp.clear();
	}

	if (tokens.size())
	{
		if (tokens[0] == "NICK" || tokens[0] == "nick")
			nick(tokens, fd);
		if (tokens[0] == "PASS" || tokens[0] == "pass")
			pass(tokens, fd);

}
