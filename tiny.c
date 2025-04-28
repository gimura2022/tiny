#include <stdio.h>	/* tiny - very small messsenger */
#include <stdlib.h>	/* you need to have there headers in your system to compile tiny */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define pexit(x) { perror(x); exit(-1); }	/* for print the error from a errno and exit */

int main(int argc, char* argv[])
{
	int server, user, opt;		/* a variables for a sockets (server, user) and for setsockopt */
	char buffer[1024] = {0};	/* a buffer for messages */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				/* check arguments */
		puts("usage: tiny port [-h]\n""	-h	print usage\n");	/* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				/* given -h, print usage */
	}

	struct sockaddr_in a = { AF_INET, htons(atoi(argv[1])), .sin_addr.s_addr = INADDR_ANY };
	int al               = sizeof(a);

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) pexit("socket");	/* this code wait client */
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))		pexit("setsockopt");
	if (bind(server, (struct sockaddr*) &a, al) < 0)				pexit("bind");
	if (listen(server, 1) < 0)							pexit("listen");
	if ((user = accept(server, (struct sockaddr*) &a, (socklen_t*) &al)) < 0)	pexit("accept");

	while(1) {							/* a main loop */
		if (read(user, buffer, sizeof(buffer)) <= 0) break;	/* read the client message */
		fputs(buffer, stdout);					/* print the client message */
		fgets(buffer, sizeof(buffer), stdin);			/* get a user input */
		send(user, buffer, strlen(buffer), 0);			/* send a user input to the client */
	}

	close(user);	/* the client connection interrupted, close a user socket and a server socket */
	close(server);

	return 0;	/* exit with success exit code */
}
