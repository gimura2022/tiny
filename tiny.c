#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define pexit(x) { perror(x); exit(-1); }

int main(int argc, char* argv[])
{
	int server, user;
	char buffer[1024]    = {0};
	int al               = sizeof(struct sockaddr_in);
	int opt              = 1;
	struct sockaddr_in a = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, htons(atoi(argv[1]))};

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {
		puts("usage: tiny port [-h]\n""	-h	print usage\n");
		exit(argc < 2 ? -2 : EXIT_SUCCESS);
	}
	
	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)				pexit("socket");
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))		pexit("setsockopt");
	if (bind(server, (struct sockaddr*) &a, al) < 0)				pexit("bind");
	if (listen(server, 1) < 0)							pexit("listen");
	if ((user = accept(server, (struct sockaddr*) &a, (socklen_t*) &al)) < 0)	pexit("accept");

	while(1) {
		if (read(user, buffer, sizeof(buffer)) <= 0) break;
		fputs(buffer, stdout);
		fgets(buffer, sizeof(buffer), stdin);
		send(user, buffer, strlen(buffer), 0);
	}
	
	close(user);
	close(server);

	return 0;
}
