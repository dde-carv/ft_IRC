#include "../../inc/Server.hpp"

//* Syntax: invite/INVITE <nickname> #<channel>
//* Ex.: invite dicarval #bocal

//* ERR_NEEDMOREPARAMS		(461)
//* ERR_NOSUCHCHANNEL		(403)
//* ERR_NOTONCHANNEL		(442)
//* ERR_USERONCHANNEL		(443)
//* ERR_NOSUCHNICK			(401)
//* ERR_CHANOPRIVSNEEDED	(482)
//* ERR_CHANNELISFULL		(471)
//* RPL_INVITING			(341)

//!! ADD "invite" TO SERVER CLASS (Server::)
//!! ADD "splitCMD", "senderr", "getClient", "getChannel", "getClientNick" TO SERVER CLASS
//!! ADD "getNick" TO CLIENT CLASS

void	invite(std::string &cmd, int &fd)
{
	std::vector<std::string> sCmd = splitCmd(cmd);
	if (sCmd.size() < 3) //? ERR_NEEDMOREPARAMS (461) if there are not enough parameters
	{
		senderr(461, getClient(fd)->getNick(), fd, ": Not enough parameters!\r\n");
		return ;
	}
	std::string chanName = sCmd[2].substr(1); //? The substr start position is 1 so it skips the '#' character
	if ((sCmd[2][0] != '#') || !getChannel(chanName)) //? ERR_NOSUCHCHANNEL (403) if there is not a '#' character and/or the channel does not exists
	{
		senderr(403, chanName, fd, ": There is no such channel!\r\n");
		return ;
	}
	if (!(getChannel(chanName)->getClient(fd)) && !(getChannel(chanName)->getAdmin(fd))) //? ERR_NOTONCHANNEL (442) if the client is not on the channel
	{
		senderr(442, chanName, fd, ": You are not in that channel!\r\n");
		return ;
	}
	if (getChannel(chanName)->getClientInChannel(sCmd[1])) //? ERR_USERONCHANNEL (443) if the given nickname is already on the channel
	{
		senderr(443, getClient(fd)->getNick(), chanName, fd, ": The client is already in the channel!\r\n");
		return ;
	}
	Client *clt = getClientNick(sCmd[1]);
	if (!clt) //? ERR_NOSUCHNICK (401) if the given nickname is not found
	{
		senderr(401, sCmd[1], fd, ": No such nick!\r\n");
		return ;
	}
	if (getChannel(chanName)->getInviteOnly() && !(getChannel(chanName)->getAdmin(fd))) //? ERR_CHANOPRIVSNEEDED (482) if the sender is not an operator
	{
		senderr(482, getChannel(chanName)->getClient(fd)->getNick(), sCmd[1], fd, ": You are not a channel operator!\r\n");
		return ;
	}
	if (getChannel(chanName)->getLimit() && getChannel(chanName)->getClientsNumber() >= getChannel(chanName)->getLimit()) //? ERR_CHANNELISFULL (471) if channel has limit and is full
	{
		senderr(471, getChannel(chanName)->getClient(fd)->getNick(), chanName, fd, ": Can not invite to channel. Channel is full!");
		return ;
	}
	//? RPL_INVITING (341) if the invite was successfully sent
	clt->AddChannelInvite(chanName);
	std::string rpl1 = (": 301 " + getClient(fd)->getNick() + " " + clt->getNick() + " " + sCmd[2] + "\r\n");
	sendResp(rpl1, fd);
	std::string rpl2 = (":" + clt->getHostName() + " INVITE " + clt->getNick() + " " + sCmd[2] + "\r\n");
	sendResp(rpl2, fd);
}
