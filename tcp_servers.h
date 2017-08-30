#ifndef __SERVERS_TCP_H
#define __SERVERS_TCP_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include "kernel_list.h"


#define SEND  0
#define RECV  1

#define BUFSIZE 118784

typedef struct list_readfd
{
	int fd;
	struct sockaddr_in clinet;
	bool type;
	struct list_head list;
}list_readfd;

int tcp_servers(int port);
list_readfd *init_list(void);
list_readfd *new_readfd(int readfd, struct sockaddr_in *addr);


#endif
