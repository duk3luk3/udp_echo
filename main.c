#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFLEN 1024

static int _run = 1;

void sighandler(int sig) {
	if (sig == SIGINT) {
		_run = 0;
	} else {
		fprintf(stderr, "Caught signal %d", sig);
	}
}

int main(int argc, char** args) {
	if (argc < 2) {
		printf("Usage: %s PORT\n", args[0]);
		return -1;
	} else {
		signal(SIGINT, &sighandler);

		int port = atoi(args[1]);
		struct sockaddr_in addr_srv, addr_cli;
		int sock;
		int addr_len;
		ssize_t readlen;
		char buf[BUFLEN];

		if ((sock = socket(AF_INET, SOCK_DGRAM, 0))==-1) {
			perror("Error allocating socket");
			return -1;
		}

		memset(&addr_srv, 0, sizeof(struct sockaddr_in));

		addr_srv.sin_family = AF_INET;
		addr_srv.sin_port = htons(port);
		addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr *)&addr_srv,
					sizeof(struct sockaddr_in))==-1) {
			perror("Error binding socket");
			return -1;
		}

		while (_run) {
			if ((readlen=recvfrom(sock, buf, BUFLEN, 0,
				(struct sockaddr *)&addr_cli, &addr_len)) == -1)
			{
				perror("Error receiving");
				return -1;
			} else if (readlen > 0) {
				sendto(sock, buf, readlen, MSG_NOSIGNAL,
				(struct sockaddr *)&addr_cli, addr_len);
			}
		}

		close(sock);
		return 0;
	}
}


