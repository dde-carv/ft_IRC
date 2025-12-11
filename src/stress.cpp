# include <iostream>

# include <pthread.h>

# include <string>

# include <cstring>

# include <math.h>

# include <cmath>

# include <fstream>

# include <sstream>

# include <unistd.h>

# include <sys/types.h>

# include <sys/socket.h>

# include <sys/select.h>

# include <sys/epoll.h>

# include <sys/wait.h>

# include <sys/stat.h>

# include <string.h>

# include <netdb.h>

# include <errno.h>

# include <arpa/inet.h>

# include <poll.h>

# include <fcntl.h>

# include <signal.h>

# include <dirent.h>



void	*stress(void *p)

{

	pthread_mutex_t	*lock = (pthread_mutex_t *)p;

	pthread_mutex_lock(lock);

	pthread_mutex_unlock(lock);

	system("nc localhost 6697 && pwd");


	return (NULL);

}



int	main(void)

{

	pthread_t	id[1025];

	pthread_mutex_t	lock;



	pthread_mutex_lock(&lock);

	for (int i = 0; i < 1025; i++)

	{

		pthread_create(&id[i], NULL, stress, &lock);

	}

	pthread_mutex_unlock(&lock);

	for (int i = 0; i < 1025; i++)

	{

		pthread_join(id[i], NULL);

	}

	pthread_mutex_destroy(&lock);

	return (0);

}

