//* Syntax: mode/MODE <target> +/-<mode>(itkol) <params>

#include "../../inc/Server.hpp"

std::string modeToAppend(std::string chain, char opera, char mode)
{
	std::stringstream ss;

	char last ='\0';
	for (size_t i = 0; i < chain.size(); i++)
	{
		if (chain[i] == '+' || chain[i] == '-')
			last = chain[i];
	}
	if (last != opera)
		ss << opera << mode;
	else
		ss << mode;
	return ss.str();
}

bool validLimit(std::string limit)
{
	return (!(limit.find_first_not_of("0123456789")!= std::string::npos) && std::atoi(limit.c_str()) > 0);
}

std::string Server::limitMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain)
{
	std::string param;
	std::string limit;

	if (opera == '+')
	{
		if (tokens.size() > pos)
		{
			limit = tokens[pos++];
			if (!validLimit(limit))
				sendRsp(ERR_INVALIDMODEPARM(channel->getName(), std::string("(l)")), fd);
			else
			{
				if (!channel->getModeAtIndex(4))
					channel->setModeAtIndex(4, true);
				channel->setLimitOfClients(std::atoi(limit.c_str()));
				if (!args.empty())
					args += " ";
				args += limit;
				param = modeToAppend(chain, opera, 'l');
			}
		}
		else
			sendRsp(ERR_NEEDMODEPARM(channel->getName(), std::string("(l)")), fd);
	}
	else if (opera == '-' && channel->getModeAtIndex(4))
	{
		channel->setModeAtIndex(4, false);
		channel->setLimitOfClients(0);
		param = modeToAppend(chain, opera, 'l');
	}
	return param;
}

std::string Server::operatorPrivilegeMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain)
{
	std::string param;
	std::string user;

	if (tokens.size() > pos)
		user = tokens[pos++];
	else
	{
		sendRsp(ERR_NEEDMODEPARM(channel->getName(), std::string("(o)")), fd);
		return param;
	}
	if (!channel->getClientInChannel(user))
	{
		sendRsp(ERR_USERNOTINCHANNEL(getClientFd(fd)->getNickName() ,user, channel->getName()), fd);
		return param;
	}
	if (opera == '+')
	{
		if (!channel->getModeAtIndex(3))
			channel->setModeAtIndex(3, true);
		if (channel->clientToAdmin(user))
		{
			if (!args.empty())
				args += " ";
			args += user;
			param = modeToAppend(chain, opera, 'o');
		}
	}
	else if (opera == '-')
	{
		if (channel->getModeAtIndex(3))
			channel->setModeAtIndex(3, false);
		if (channel->adminToClient(user))
		{
			if (!args.empty())
				args += " ";
			args += user;
			param = modeToAppend(chain, opera, 'o');
		}
	}
	return param;
}

bool validPassword(std::string password)
{
	if (password.empty())
		return false;
	for (size_t i = 0; i < password.size(); i++)
	{
		if (!std::isalnum(password[i]) && password[i] != '_')
			return false;
	}
	return true;
}

std::string Server::passwordMode(std::vector<std::string> tokens, Channel *channel, size_t &pos, char opera, int fd, std::string &args, std::string chain)
{
	std::string param;
	std::string pass;

	if (tokens.size() > pos)
		pass = tokens[pos++];
	else
	{
		sendRsp(ERR_NEEDMODEPARM(channel->getName(), std::string("(k)")), fd);
		return param;
	}
	if (!validPassword(pass))
	{
		sendRsp(ERR_INVALIDMODEPARM(channel->getName(), std::string("(k)")), fd);
		return param;
	}
	if (opera == '+')
	{
		if (channel->getModeAtIndex(2))
		{
			sendRsp(ERR_KEYSET(channel->getName()), fd);
			return param;
		}
		channel->setModeAtIndex(2, true);
		channel->setPassword(pass);
		if (!args.empty())
			args += " ";
		args += pass;
		param = modeToAppend(chain, opera, 'k');
	}
	else if (opera == '-' && channel->getModeAtIndex(2))
	{
		if (pass == channel->getPassword())
		{
			channel->setModeAtIndex(2, false);
			channel->setPassword("");
			param = modeToAppend(chain, opera, 'k');
		}
		else
			sendRsp(ERR_INVALIDMODEPARM(channel->getName(), std::string("(k)")),fd);
	}
	return param;
}

std::string topicRestriction(Channel *channel, char opera, std::string chain)
{
	std::string param;

	if (opera == '+' && !channel->getModeAtIndex(1))
	{
		channel->setModeAtIndex(1, true);
		channel->setTopicRestriction(true);
		param = modeToAppend(chain, opera, 't');
	}
	else if (opera == '-' && channel->getModeAtIndex(1))
	{
		channel->setModeAtIndex(1, false);
		channel->setTopicRestriction(false);
		param = modeToAppend(chain, opera, 't');
	}
	return param;
}

std::string inviteOnly(Channel *channel, char opera, std::string chain)
{
	std::string param;

	if (opera == '+' && !channel->getModeAtIndex(0))
	{
		channel->setModeAtIndex(0, true);
		channel->setInviteOnly(1);
		param = modeToAppend(chain, opera, 'i');
	}
	else if (opera == '-' && channel->getModeAtIndex(0))
	{
		channel->setModeAtIndex(0, false);
		channel->setInviteOnly(0);
		param = modeToAppend(chain, opera, 'i');
	}
	return param;
}

void	Server::mode(std::vector<std::string> &cmd, int &fd)
{
	std::string channelName;
	std::string setMode;
	std::stringstream modeChain;
	std::string args;
	Channel *channel;
	Client *client = getClientFd(fd);
	char opera = '\0';

	if (cmd.size() < 2)
		return sendRsp(ERR_NEEDMOREPARAMS(client->getNickName()), fd);
	channelName = cmd[1];
	channel = getChannel(channelName.substr(1));
	if (channelName[0] != '#' || !channel)
		return sendRsp(ERR_NOSUCHCHANNEL(client->getNickName(), channelName), fd);
	else if (!channel->getClient(fd) && !channel->getAdmin(fd))
		return sendRsp(ERR_NOTONCHANNEL(client->getNickName(), channelName), fd);
	else if (cmd.size() == 2)
		return sendRsp(RPL_CHANNELMODES(client->getNickName(), channelName, channel->getModes()) + \
			RPL_CREATIONTIME(client->getNickName(), channelName, channel->getChannelCreation()), fd);
	else if (!channel->getAdmin(fd))
		return sendRsp(ERR_CHANOPRIVSNEEDED(channelName), fd);

	setMode = cmd[2];
	std::vector<std::string> tokens;
	if (cmd.size() > 3)
		tokens.assign(cmd.begin() + 3, cmd.end());

	if (channel)
	{
		size_t pos = 0;
		for(size_t i = 0; i < setMode.size(); i++)
		{
			if (setMode[i] == '+' || setMode[i] == '-')
				opera = setMode[i];
			else
			{
				if (setMode[i] == 'i')
					modeChain << inviteOnly(channel, opera, modeChain.str());
				else if (setMode[i] == 't')
					modeChain << topicRestriction(channel, opera, modeChain.str());
				else if (setMode[i] == 'k')
					modeChain << passwordMode(tokens, channel, pos, opera, fd, args, modeChain.str());
				else if (setMode[i] == 'o')
					modeChain << operatorPrivilegeMode(tokens, channel, pos, opera, fd, args, modeChain.str());
				else if (setMode[i] == 'l')
					modeChain << limitMode(tokens, channel, pos, opera, fd, args, modeChain.str());
				else
					sendRsp(ERR_UNKNOWNMODE(client->getNickName(), channelName, setMode[i]), fd);
			}
		}
	}
	//std::string chain = modeChain.str();
	if (modeChain.str().empty())
		return ;
	channel->sendToAll(RPL_CHANGEMODE(client->getHostname(), channelName, modeChain.str(), args));
}
