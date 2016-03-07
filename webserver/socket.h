#ifndef __SOCKET_H__
#define __SOCKET_H__

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/wait.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <string.h>
	#include <signal.h>
	#include <regex.h>

	int create_server(int port);
	int start(int sockfd);


#endif
