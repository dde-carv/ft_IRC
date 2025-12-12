#include "Server.hpp"

void	Server::topic(std::vector<std::string> &tokens, int fd)
{
	tokens.erase(tokens.begin());
	if ((tokens.size() == 1 && tokens[1] == ":") || tokens.size() == 0)
		return (sendResponse(ERR_NOTENOUGHPARAM(this->getClient(fd)->getNickName(), fd)));
	if (*(tokens[0].begin()) == '#')
		tokens[0].erase(tokens[0].begin());
	else
		return (sendResponse(ERR_CHANNELNOTFOUND(this->getClient(fd)->getNickName(), tokens[0]), fd));
	if (!this->getChannel(tokens[0])->getClient(fd) && !this->getChannel(tokens[0])->getAdmin(fd))
		return (sendResponse(ERR_NOTONCHANNEL(this->getClient(fd)->getNickName(), "#" + tokens[0]), fd));

}
