#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <netinet/in.h>

#define pexit(x) { perror(x); exit(1); }

int main(int argc, char* argv[])
{
	int server_fd, user_fd, c, readcount;
	struct sockaddr_in address;
	char buffer[1024] = {0};
	int addrlen       = sizeof(address);
	uint16_t port     = 8080;
	int opt           = 1;

	while ((c = getopt(argc, argv, "hp:")) != -1) switch (c) {
	case 'p':
		port = atoi(optarg);
		break;

	case 'h':
		puts("usage: tiny [-h] [-p port]\n"
				"	-h	print usage\n"
				"	-p	port, default 8080");
		exit(EXIT_SUCCESS);

	case '?':
		exit(2);
	}

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		pexit("socket");
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		pexit("setsockopt");
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
		pexit("bind");

	if (listen(server_fd, 3) < 0)
		pexit("listen");

	if ((user_fd = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0)
		pexit("accept");

	while(1) {
		readcount = read(user_fd, buffer, sizeof(buffer));

		if (readcount <= 0)
			break;

		fputs(buffer, stdout);
		
		fgets(buffer, sizeof(buffer), stdin);
		send(user_fd, buffer, strlen(buffer), 0);
	}
	
	close(user_fd);
	close(server_fd);

	return 0;
}
