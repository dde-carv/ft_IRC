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
