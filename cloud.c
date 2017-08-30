#include <stdio.h>
#include "tcp_servers.h"

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
	unsigned char *msg = malloc(BUFSIZE);
	while(1)
	{
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		list_for_each_entry(tmp, &head->list, list)
		{
			FD_SET(tmp->fd, &readfds);
			fd_max = tmp->fd>fd_max? tmp->fd: fd_max;
			printf("Welcome [%s:%d] %d\n", 
				inet_ntoa(tmp->clinet.sin_addr),
				ntohs(tmp->clinet.sin_port), tmp->type);
		}

		select(fd_max+1, &readfds, NULL, NULL, 0);
		if(FD_ISSET(fd, &readfds))
		{
			bzero(&client, len);
			int fdtmp = accept(fd, (struct sockaddr *)&client, &len);
			printf(" [%s:%d]\n", 
				inet_ntoa(client.sin_addr),
				ntohs(client.sin_port));
			list_readfd *new = new_readfd(fdtmp, &client);
			list_add_tail(&new->list, &head->list);
		}

		list_for_each_entry(tmp, &head->list, list)
		{
			if(FD_ISSET(tmp->fd, &readfds))
			{
				bzero(msg, BUFSIZE);
				int nread, total = 0;
				unsigned char *p = msg;
				int ret;
				while((ret=read(tmp->fd, msg, 1)) == 0);
				printf("msg: %s\n", msg);
				if(*msg == '0')
				{
#ifdef DEBUG
				printf("[%d]\n", __LINE__);
#endif
					while(1)
					{
						nread = read(tmp->fd, p+total, 8-total);
						total += nread;
						if(total == 8)
							break;
					}
				printf("msg: %s\n", msg);
				}
				else if(*msg == '1')
				{
#ifdef DEBUG
				printf("[%d]\n", __LINE__);
#endif
					while(1)
					{
						nread = read(tmp->fd, p+total, BUFSIZE-total);
						total += nread;
						if(total == BUFSIZE)
							break;
						
					}
				}
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
				if(strstr(msg, "send\r\n\r\n"))
				{
					tmp->type = SEND;
					continue;
				}

				if(tmp->type == SEND)
				{
					list_readfd *ctrl;
					list_for_each_entry(ctrl, &head->list, list)
					{
						if(ctrl->type == RECV)
							write(ctrl->fd, msg, BUFSIZE);
					}
				}
			}
		}
	}

	return 0;
}
