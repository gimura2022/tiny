#include <stdio.h>	/* rsae - small rsa encoder for tiny */
#include <stdlib.h>	/* you need to have there headers in your system to compile tiny */
#include <string.h>	/* for use with tiny start command like "rsae key.pub | tiny | rsad key.priv" */
#include <err.h>

#include "trsa.h"	/* tiny rsa header */

int main(int argc, char* argv[])
{
	unsigned long long int c;	/* encryption result */
	struct rsakey pubkey;		/* pubkey */
	char buffer[1024] = {0};	/* buffer for user input */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				 /* check arguments */
		puts("usage: rsae pubkey_file [-h]\n""	-h	print usage\n"); /* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				 /* given -h, print usage */
	}

	pubkey = readkey(argv[1]);	/* read key from file */

	while (1) {						/* main loop */
		fgets(buffer, sizeof(buffer), stdin);		/* get user input */

		for (int i = 0; buffer[i] != '\0'; i++) {	/* encryption loop */
			c = applykey(&pubkey, buffer[i]);	/* encrypt charcter */
			printf("%llu\n", c);			/* send charcter */
		}
	}
	
	return 0;	/* exit */
}
