#include "Server.hpp"

void	Server::splitPrivmsg(std::vector<std::string> &tokens, \
std::vector<std::string> &recipients, \
std::vector<std::string> &channPrivmsg, std::string &msg, int fd)
{
	std::string temp;

	for (size_t i = 0; i < tokens[0].size(); i++)
	{
		if (tokens[0][i] != ',')
			temp += tokens[0][i];
		else
		{
			if (temp.empty())
				continue ;
			if (*(temp.begin()) == '#')
			{
				temp.erase(temp.begin());
				if (temp.empty())
					continue ;
				channPrivmsg.push_back(temp);
			}
			else
				recipients.push_back(temp);
			temp.clear();
		}
	}
	msg = findMsg(tokens);
}

void	Server::privmsg(std::vector<std::string> &tokens, int fd)
{
	std::vector<std::string> recipients;
	std::vector<std::string> channRecipients;
	std::string msg;

	tokens.erase(tokens.begin());
	splitPrivmsg(tokens, recipients, channRecipients, msg, fd);
	if (recipients.empty() && channRecipients.empty())
		return (sendResponse(ERR_NORECIPIENT(getClientFd(fd)->getNickName()), fd));
	if (msg.empty())
		return (sendResponse(ERR_NOTEXTTOSEND(getClientFd(fd)->getNickName()), fd));
	if (recipients.size() + channRecipients.size() > 10)
		return (sendResponse(ERR_TOOMANYTARGETS(getClientFd(fd)->getNickName()), fd));
	std::string msgToSend = msg + "\r\n";
	for (size_t i = 0; i < recipients.size(); i++)
	{
		if (this->getClientNick(recipients[i]))
		{
			int fdRecipient = this->getClientNick(recipients[i])->getFd();
			sendResponse(":" + this->getClient(fd)->getNickName() + "!~" + this->getClient(fd)->getUserName() + \
			"@localhost PRIVMSG " + recipients[i] + " :" + msgToSend, fdRecipient);
		}
		else
			sendResponse(ERR_NOSUCHNICK(recipients[i]), fd);
	}
	for (size_t i = 0; i < channRecipients.size(); i++)
	{
		if (this->getChannel(channRecipients[i]))
			this->getChannel(channRecipients[i])->sendToAll(":" + this->getClient(fd)->getNickName() + "!~" + \
			this->getClient(fd)->getUserName() + "@localhost PRIVMSG #" + channRecipients[i] + " :" + msgToSend, fd);
		else
			sendResponse(ERR_NOSUCHNICK(channRecipients[i]), fd);
	}
}
