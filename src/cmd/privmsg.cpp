#include "../../inc/Server.hpp"

//* Syntax: privmsg/PRIVMSG <recipient>,<recipient> :message
//* Ex.: privmsg #bocal,dde-carv :Hello!

//* ERR_NOSUCHNICK		(401)
//* ERR_TOOMANYTARGETS	(407)
//* ERR_NORECIPIENT		(411)
//* ERR_NOTEXTTOSEND	(412)

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
		return (sendRsp(ERR_NORECIPIENT(this->getClientFd(fd)->getNickName()), fd));
	if (msg.empty())
		return (sendRsp(ERR_NOTEXTTOSEND(this->getClientFd(fd)->getNickName()), fd));
	if (recipients.size() + channRecipients.size() > 10)
		return (sendRsp(ERR_TOOMANYTARGETS(this->getClientFd(fd)->getNickName()), fd));
	std::string msgToSend = msg + CRLF;
	for (size_t i = 0; i < recipients.size(); i++)
	{
		if (this->getClientNick(recipients[i]))
		{
			int fdRecipient = this->getClientNick(recipients[i])->getFd();
			sendRsp(":" + this->getClientFd(fd)->getNickName() + "!~" + this->getClientFd(fd)->getUserName() + \
			"@localhost PRIVMSG " + recipients[i] + " :" + msgToSend, fdRecipient);
		}
		else
			sendRsp(ERR_NOSUCHNICK(recipients[i]), fd);
	}
	for (size_t i = 0; i < channRecipients.size(); i++)
	{
		if (this->getChannel(channRecipients[i]))
			this->getChannel(channRecipients[i])->sendToAll(":" + this->getClientFd(fd)->getNickName() + "!~" + \
			this->getClientFd(fd)->getUserName() + "@localhost PRIVMSG #" + channRecipients[i] + " :" + msgToSend);
		else
			sendRsp(ERR_NOSUCHCHANNEL(channRecipients[i], this->getClientFd(fd)->getNickName()), fd);
	}
}
