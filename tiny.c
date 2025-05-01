#include <stdio.h>	/* tiny - tcp messenger server */
#include <stdlib.h>	/* you need to have libc and posix headers */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define pexit(x) { perror(x); exit(-1); }	/* for print the error from a errno and exit */

int main(int argc, char* argv[])
{
	int server, user, opt, readsize; /* variables for sockets (server, user) and for setsockopt */
	char buffer[1024] = {0};	 /* buffer for messages */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				/* check arguments */
		puts("usage: tiny port [-h]\n""	-h	print usage");		/* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				/* given -h, print usage */
	}

	struct sockaddr_in a = { AF_INET, htons(atoi(argv[1])), .sin_addr.s_addr = INADDR_ANY };
	int al               = sizeof(a);

	if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) pexit("socket");	/* this code wait client */
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))		pexit("setsockopt");
	if (bind(server, (struct sockaddr*) &a, al) < 0)				pexit("bind");
	if (listen(server, 1) < 0)							pexit("listen");
	if ((user = accept(server, (struct sockaddr*) &a, (socklen_t*) &al)) < 0)	pexit("accept");

	while (!feof(stdin)) {	/* main loop, if stdin ends, exit */
		if ((readsize = read(user, buffer, sizeof(buffer))) <= 0) break;
		buffer[readsize] = '\0';				/* read client message */
		fputs(buffer, stdout); fflush(stdout);			/* print client message */
		fgets(buffer, sizeof(buffer), stdin);			/* get user input */
		send(user, buffer, strlen(buffer), 0);			/* send user input to client */
	}

	close(user);	/* the client connection interrupted, close user socket and server socket */
	close(server);

	return 0;	/* exit with success exit code */
}
