#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

static void host(uint16_t port)
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("server listen at %u...", port);
	fflush(stdout);
	
	if ((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}
	
	printf("connected\n");
	
	while(1) {
		int valread = read(new_socket, buffer, BUFFER_SIZE);

		if (valread <= 0) {
			printf("disconect\n");
			break;
		}

		buffer[valread] = '\0';
		
		printf("otherside: %s", buffer);
		
		printf("you: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		send(new_socket, buffer, strlen(buffer), 0);
	}
	
	close(new_socket);
	close(server_fd);
}

static void client(const char* address, uint16_t port)
{
	int sock;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton");
		exit(EXIT_FAILURE);
	}

	printf("connect to server at %s:%u...", address, port);
	fflush(stdout);
	
	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		exit(EXIT_FAILURE);
	}
	
	printf("success\n");
	
	while(1) {
		printf("you: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		
		send(sock, buffer, strlen(buffer), 0);
		
		int valread = read(sock, buffer, BUFFER_SIZE);

		if (valread <= 0) {
			printf("disconnect\n");
			break;
		}

		buffer[valread] = '\0';
		
		printf("otherside: %s", buffer);
	}
	
	close(sock);
}

#define USAGE_SMALL "usage: tiny [-h] [-H] [-c] [-p] [-a]\n"
#define USAGE \
	"	-h	print usage\n" \
	"	-H	start tiny in host mode\n" \
	"	-c	start tiny in client mode\n" \
	"	-p	specify port\n" \
	"	-a	specify address\n"

static void usage(FILE* stream, bool small)
{
	fprintf(stream, small ? USAGE_SMALL : USAGE_SMALL USAGE);
}

int main(int argc, char* argv[])
{
	enum {
		MODE_UNSPECIFIED = 0,
		MODE_HOST,
		MODE_CLIENT,
	} mode              = MODE_UNSPECIFIED;
	uint16_t port       = 8080;
	const char* address = NULL;

	int c;
	while ((c = getopt(argc, argv, "Hhcp:a:")) != -1) switch (c) {
	case 'H':
		mode = MODE_HOST;
		break;

	case 'c':
		mode = MODE_CLIENT;
		break;

	case 'p':
		port = atoi(optarg);
		break;

	case 'a':
		address = optarg;
		break;

	case 'h':
		usage(stdout, false);
		exit(EXIT_SUCCESS);

	case '?':
		fprintf(stderr, "tiny: invalid option\n");
		usage(stderr, true);
		exit(EXIT_FAILURE);
	}

	switch (mode) {
	case MODE_CLIENT:
		if (address == NULL) {
			fprintf(stderr, "tiny: host address not specified\n");
			usage(stderr, true);
			exit(EXIT_FAILURE);
		}

		client(address, port);

		break;

	case MODE_HOST:
		host(port);
		break;

	case MODE_UNSPECIFIED:
		fprintf(stderr, "tiny: working mode not specified\n");
		usage(stderr, true);
		exit(EXIT_FAILURE);
	}

	return 0;
}
