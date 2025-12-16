//* Syntax: invite/INVITE <nickname> #<channel>

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_NOTONCHANNEL		(442)
//* ERR_USERONCHANNEL		(443)
//* ERR_NOSUCHNICK			(401)
//* ERR_CHANOPRIVSNEEDED	(482)
//* ERR_CHANNELISFULL		(471)
//* RPL_INVITING			(341)

#include "../../inc/Server.hpp"

void	Server::invite(std::vector<std::string> &cmd, int &fd)
{
	if (cmd.size() < 3)
	{
		sendRsp(ERR_NEEDMOREPARAMS(getClientFd(fd)->getNickName()), fd);
		return ;
	}
	std::string chanName = cmd[2].substr(1);
	if ((cmd[2][0] != '#') || !getChannel(chanName))
	{
		sendRsp(ERR_NOSUCHCHANNEL(getClientFd(fd)->getNickName(), chanName), fd);
		return ;
	}
	if (!(getChannel(chanName)->getClient(fd)) && !(getChannel(chanName)->getAdmin(fd)))
	{
		sendRsp(ERR_NOTONCHANNEL(getClientFd(fd)->getNickName(), chanName), fd);
		return ;
	}
	if (getChannel(chanName)->getClientInChannel(cmd[1]))
	{
		sendRsp(ERR_USERONCHANNEL(getClientFd(fd)->getNickName(), chanName), fd);
		return ;
	}
	Client *clt = getClientNick(cmd[1]);
	if (!clt)
	{
		sendRsp(ERR_NOSUCHNICK(cmd[1]), fd);
		return ;
	}
	if (getChannel(chanName)->getInviteOnly() && !(getChannel(chanName)->getAdmin(fd)))
	{
		sendRsp(ERR_CHANOPRIVSNEEDED(chanName), fd);
		return ;
	}
	if (getChannel(chanName)->getLimitOfClients() && getChannel(chanName)->getNumberOfClients() >= getChannel(chanName)->getLimitOfClients())
	{
		sendRsp(ERR_CHANNELISFULL(getClientFd(fd)->getNickName(), chanName), fd);
		return ;
	}
	//? RPL_INVITING (341) if the invite was successfully sent
	clt->AddChannelInvite(chanName);
	std::string rpl1 = (": 301 " + getClientFd(fd)->getNickName() + " " + clt->getNickName() + " " + cmd[2] + "\r\n");
	sendRsp(rpl1, fd);
	std::string rpl2 = (":" + clt->getHostname() + " INVITE " + clt->getNickName() + " " + cmd[2] + "\r\n");
	sendRsp(rpl2, clt->getFd());
}
