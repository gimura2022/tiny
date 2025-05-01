#include <stdio.h>	/* rsad - small rsa decoder for tiny */
#include <stdlib.h>	/* you need to have libc, posix and rsa.h headers */
#include <string.h>
#include <err.h>

#include "rsa.h"	/* tiny rsa header */

int main(int argc, char* argv[])
{
	unsigned long long int c;	/* encryption result */
	int i = 0;			/* varable for counting char */
	char* s;			/* pointer for strtok */
	struct rsakey privkey;		/* privkey */
	char buffer[1024] = {0};	/* buffer for user input */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				 /* check arguments */
		puts("usage: rsad privkey_file [-h]\n""	-h	print usage"); /* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				 /* given -h, print usage */
	}

	privkey = readkey(argv[1]);	/* read key from file */

	while (fgets(buffer, sizeof(buffer), stdin)) for (s = strtok(buffer, ":"); s != NULL;
			s = strtok(NULL, ":")) {		/* read line and separate it by : */
		c = applykey(&privkey, atoll(s)) - i++;		/* decrypt char */
		printf("%c", (char) c); fflush(stdout);		/* send charcter */
	}
	
	return 0;	/* exit */
}
