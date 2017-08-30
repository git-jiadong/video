#include "tcp_servers.h"

int tcp_servers(int port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd == -1)
	{
		perror("socket failed: ");	
		return -1;
	}

	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	bzero(&addr, len);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	printf("ip:%s port: %d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if(bind(fd, (struct sockaddr *)&addr, len) == -1)
	{
		perror("bind() failed: ");
		return -1;
	}

	if(listen(fd, 3) == -1)
	{
		perror("listen() failed: ");
		return -1;
	}

	return fd;
}

list_readfd *init_list(void)
{
	list_readfd *head = malloc(sizeof(list_readfd));
	if(head != NULL)
	{
		INIT_LIST_HEAD(&head->list);
	}

	return head;
}

list_readfd *new_readfd(int readfd, struct sockaddr_in *addr)
{
	list_readfd *new = malloc(sizeof(list_readfd));
	if(new != NULL)
	{
		INIT_LIST_HEAD(&new->list);
		new->fd = readfd;
		new->clinet = *addr;
		new->type = RECV;
	}

	return new;
}
