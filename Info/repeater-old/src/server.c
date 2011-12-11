#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h> // memset
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <main.h>
#include <server.h>

#define MAX_CONNECTION 100
#define BUF_SIZE 128

int create_socket_stream(const char* hostname, const char* servname);
int get_sock_info(int sock, char* buffer);
void lisenquit_sig_handler(int signo);

volatile sig_atomic_t run;

int lisen_connection(int listening_socket)
{
	struct sockaddr_in address;
	socklen_t length;
	int sockets[MAX_CONNECTION];
	int socketc = 0;
	int socket;
	char buffer[BUF_SIZE];
	
	/* accept is an exemple of interruptible function, that should not be restarted on SIGQUIT, SIGINT, … */
	install_sig_handler(SIGQUIT, 0, NULL, lisenquit_sig_handler);
	install_sig_handler(SIGINT, 0, NULL, lisenquit_sig_handler);
	install_sig_handler(SIGTERM, 0, NULL, lisenquit_sig_handler);
	
	sigset_t exitsigset;
	sigemptyset(&exitsigset);
	sigaddset(&exitsigset, SIGKILL);
	sigaddset(&exitsigset, SIGTERM);
	sigaddset(&exitsigset, SIGINT);
	
	int flags = fcntl (listening_socket, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(listening_socket, F_SETFL, flags);
	
	run = 1;
	
	fd_set fdset;
	while (run) { // Control ?
		sprintf(lgbf, "LISEN ON"); servlog(LOG_INFO);
		FD_ZERO(&fdset);
		FD_SET(listening_socket, &fdset);
		sprintf(lgbf, "\tlistenning socket"); servlog(LOG_INFO);
		for (int i = 0; i < socketc; i++) {
			FD_SET(sockets[i], &fdset);
			sprintf(lgbf, "\tsocket %i", i); servlog(LOG_INFO);
		}
		if (select(FD_SETSIZE, &fdset, NULL, NULL, NULL) <= 0) {
			sprintf(lgbf, "select: %s", strerror(errno));
			servlog(LOG_WARNING);
			continue;
		}
		if (FD_ISSET(listening_socket, &fdset)) {
			length = sizeof(struct sockaddr_in);
			socket = accept(listening_socket, (struct sockaddr*)(&address), &length);
			if (socket != -1) {
				int flags = fcntl (socket, F_GETFL);
				flags |= O_NONBLOCK;
				fcntl(socket, F_SETFL, flags);
				sockets[socketc++] = socket;
				sprintf(lgbf, "Socket %i connected", socketc); servlog(LOG_INFO);
			} else if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
				lerror(LOG_ERROR, "accept");
				return -1;
			}
		}
		for (int i = 0; i < socketc; i++) {
			if (FD_ISSET(sockets[i], &fdset)) {
				sprintf(lgbf, "Read data from socket %i", i); servlog(LOG_INFO);
				int nbyte;
				if ((nbyte = read(sockets[i], buffer, BUF_SIZE-1)) < 0) {
					if (errno != EAGAIN && errno != EWOULDBLOCK)
						lerror(LOG_WARNING, "read");
				} else if (nbyte == 0) {
					sprintf(lgbf, "Socket %i closed", i);
					sprintf(lgbf, "Socket %i moved to %i", socketc, i);
					close(sockets[i]);
					sockets[i] = sockets[--socketc];
					servlog(LOG_INFO);
				} else {
					sprintf(lgbf, "%i bytes read from socket %i", nbyte, i); servlog(LOG_INFO);
					for (int j = 0; j < socketc ; j++) {
						if (i != j) {
							if (send(sockets[j], buffer, nbyte/*strlen(buffer)*/, 0) < 0) {
								lerror(LOG_WARNING, "write");
							}
						}
					}
				}
			}
		}
	}
	
	return 0;
}

void lisenquit_sig_handler(int signo)
{
	run = 0;
}

int create_tcp_server(const char* hostname, const char* servname)
{
	int listening_socket;

	if ((listening_socket = create_socket_stream(hostname, servname)) < 0)
		return -1;

	if (listen(listening_socket, 8) < 0) {	/* (Socket, Waiting list size) */
		lerror(LOG_WARNING, "listen");
		close(listening_socket);
		return -1;
	}
	
	return listening_socket;
}

int create_socket_stream(const char* hostname, const char* servname)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;

	memset(&hints, 0, sizeof(struct addrinfo)); /* init hints */
	hints.ai_flags = AI_PASSIVE;				/* Not only loopback if hostname is null */
	hints.ai_family = AF_INET;					/* AF_UNSPEC/AF_INET/AF_INET6 */
	hints.ai_socktype = SOCK_STREAM;			/* STREAM(TCP), DGRAM(UDP), SEQPACKET(?) */
	hints.ai_protocol = 0;						/* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	s = getaddrinfo(hostname, servname, &hints, &result);
	if (s != 0) {
		fprintf(stdout, "Error: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {

		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		
		if (sfd == -1) {
			perror("socket");
			continue;
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;	/* Success */
		} else {
			perror("bind");
			fprintf(stdout, "Tried address : %s\n", inet_ntoa(((const struct sockaddr_in*)(rp->ai_addr))->sin_addr));
		}

		close(sfd);
	}
	freeaddrinfo(result);	/* No longer needed */

	if (rp == NULL) {	/* No address succeeded */
		fprintf(stdout, "Error: Could not bind\n");
		return -1;
	}

	return sfd;
}


int get_sock_info(int sock, char* buffer)
{
	struct sockaddr_in address;
	socklen_t length;

	length = sizeof(struct sockaddr_in);
	if (getsockname(sock, (struct sockaddr*)&address, &length) < 0) {
		return -1;
	}
	sprintf(buffer, "IP = %s, Port = %u\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

	return 0;
}
