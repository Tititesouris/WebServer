#ifndef __SOCKET_H__
#define __SOCKET_H__

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <string.h>

	int create_socket(void);
	int bind_socket(int sockfd, int port);
	int listen(int sockfd, int backlog_size);
	int creer_serveur(int port);
	int start(int sockfd);
	
#endif
