#include "socket.h"


int creer_serveur(int port) {
	socket(0,0,0);
	bind(0,NULL,0);
	listen(0,0);
	return port;
}