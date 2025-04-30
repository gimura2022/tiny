#include <stdio.h>	/* rsae - small rsa encoder for tiny */
#include <stdlib.h>	/* you need to have there headers in your system to compile tiny */
#include <string.h>	/* for use with tiny start command like "rsae key.pub | tiny | rsad key.priv" */
#include <err.h>

#include "rsa.h"	/* tiny rsa header */

int main(int argc, char* argv[])
{
	unsigned long long int c;	/* encryption result */
	struct rsakey pubkey;		/* pubkey */
	char buffer[1024] = {0};	/* buffer for user input */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				 /* check arguments */
		puts("usage: rsae pubkey_file [-h]\n""	-h	print usage");	 /* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				 /* given -h, print usage */
	}

	pubkey = readkey(argv[1]);	/* read key from file */

	while (fgets(buffer, sizeof(buffer), stdin)) {		/* main loop */
		for (int i = 0; buffer[i] != '\0'; i++) {	/* encryption loop */
			c = applykey(&pubkey, buffer[i] + i);	/* encrypt charcter */
			printf("%llu", c);			/* send charcter */
			if (buffer[i + 1] != '\0') printf(":");	/* put separator */
		}

		printf("\n");
		fflush(stdout);
	}
	
	return 0;	/* exit */
}
