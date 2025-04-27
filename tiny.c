/* tiny - simple serial two side messenger */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024	/* size for buffers inside code */

#define NET_FAILURE 0	/* exit code for net errors */
#define ARG_FAILURE 1	/* exit code for invalid arguments */

#define USAGE_SMALL "usage: tiny [-h] [-p port]\n"
#define USAGE \
	"	-h	to print usage\n" \
	"	-p	port\n" \

/* print usage */
static void usage(FILE* stream, bool small)
{
	fprintf(stream, small ? USAGE_SMALL : USAGE_SMALL USAGE);
}

int main(int argc, char* argv[])
{
	int server_fd, new_socket, c, readcount;
	struct sockaddr_in address;
	char buffer[BUFFER_SIZE] = {0};
	int addrlen              = sizeof(address);
	uint16_t port            = 8080;
	int opt                  = 1;

	while ((c = getopt(argc, argv, "hp:")) != -1) switch (c) {
	case 'p':
		port = atoi(optarg);
		break;

	case 'h':
		usage(stdout, false);
		exit(EXIT_SUCCESS);

	case '?':
		fprintf(stderr, "tiny: invalid option\n");
		usage(stderr, true);
		exit(ARG_FAILURE);
	}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket");
		exit(NET_FAILURE);
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(NET_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(NET_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(NET_FAILURE);
	}

	if ((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
		perror("accept");
		exit(NET_FAILURE);
	}

	printf("connected\n");

	while(true) {
		readcount = read(new_socket, buffer, BUFFER_SIZE);

		if (readcount <= 0) {
			printf("disconect\n");
			break;
		}

		buffer[readcount] = '\0';
		fputs(buffer, stdout);
		
		fgets(buffer, BUFFER_SIZE, stdin);
		send(new_socket, buffer, strlen(buffer), 0);
	}
	
	close(new_socket);
	close(server_fd);

	return 0;
}
