#include "server.hpp"

int	main(int ac, char** av)
{
	Server server;

	if (ac != 3)
	{
		std::cout << "IRC usage: " << av[0] << " <port> <password>" << std::endl;
		return 1;
	}
	std::cout << "<<<<< Server >>>>>" << std::endl;

	server.setPort(std::atoi(av[1]));
	server.setPassword(av[2]);

	try
	{
		server.serverInit();
	}
	catch(const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
