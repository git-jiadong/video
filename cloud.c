#include <stdio.h>
#include "tcp_servers.h"

int readhead(int fd, void *buf)
{
	unsigned char *tmp = (unsigned char *)buf;
	int nread, total = 0;
	while(1)
	{
		nread = read(fd, tmp+total, 1);
		if(nread == 0)
			return 0;
		total += nread;
		if(strstr(tmp, "\r\n\r\n"))
			break;
	}

	return total;
}

int reader(int fd, void *buf, int size)
{
	unsigned char *tmp = (unsigned char *)buf;
	int nread, total = 0;
	while(1)
	{
		nread = read(fd, tmp+total, size-total);
		if(nread == 0)
			return 0;
		total += nread;
		if(total == size)
			return total;
	}
}

int main(int argc, char *argv[])
{
	int fd = tcp_servers(atoi(argv[1]));
	if(fd == -1)
	{
		printf("tcp failed\n");	
		exit(-1);
	}

	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	list_readfd *head = init_list();
	int fd_max = fd;
	fd_set readfds;
	list_readfd *tmp;
	unsigned char *head_msg = malloc(256);
	unsigned char *msg = malloc(BUFSIZE);
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		list_for_each_entry(tmp, &head->list, list)
		{
			FD_SET(tmp->fd, &readfds);
			fd_max = tmp->fd>fd_max? tmp->fd: fd_max;
		}
#ifdef DEBUG
		printf("%d\n", __LINE__);
#endif
		select(fd_max+1, &readfds, NULL, NULL, 0);
		if(FD_ISSET(fd, &readfds))
		{
			bzero(&client, len);
			int fdtmp = accept(fd, (struct sockaddr *)&client, &len);
			printf("Welcome [%s:%d]\n", 
				inet_ntoa(client.sin_addr),
				ntohs(client.sin_port));
			list_readfd *new = new_readfd(fdtmp, &client);
			list_add_tail(&new->list, &head->list);
		}
#ifdef DEBUG
		printf("%d\n", __LINE__);
#endif

		list_for_each_entry(tmp, &head->list, list)
		{
			if(FD_ISSET(tmp->fd, &readfds))
			{
				bzero(head_msg, 256);
				int ret = readhead(tmp->fd, head_msg);
#ifdef DEBUG
		printf("%d\n", __LINE__);
#endif
				if(ret == 0)
				{
					/*删除断开的节点*/
					list_readfd *pos;
					pos= list_entry((tmp->list).prev, 
							 list_readfd, list);
					close(tmp->fd);
					list_del(&tmp->list);
					free(tmp);
					tmp = pos;
					continue;
				}
				if(strstr(head_msg, "send\r\n\r\n"))
				{
					tmp->type = SEND;
					continue;
				}

				if(tmp->type == SEND)
				{
					bzero(msg, BUFSIZE);
					reader(tmp->fd, msg, atoi(head_msg));
#ifdef DEBUG
		printf("%d\n", __LINE__);
#endif
					list_readfd *ctrl;
					list_for_each_entry(ctrl, &head->list, list)
					{
						if(ctrl->type == RECV)
							write(ctrl->fd, msg, atoi(head_msg));
					}
				}
			}
		}
	}

	return 0;
}
