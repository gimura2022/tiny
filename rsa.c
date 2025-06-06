#include <stdio.h>	/* trsa - tiny rsa realisation */
#include <stdlib.h>	/* you need to have libc and posix headers */
#include <err.h>

#include "rsa.h"	/* tiny rsa header */

#define pexit(x) { perror(x); exit(-1); }	/* for print the error from a errno and exit */

struct rsakey readkey(const char* filename)	/* read key from file */
{
	struct rsakey key;			/* output key */

	FILE* file = fopen(filename, "r");	/* read file */
	if (file == NULL) pexit("fopen");
	if (fscanf(file, "%lld:%lld\n", &key.a, &key.b) != 2) err(EXIT_FAILURE, "invalid keyfile format");
	fclose(file);				/* close file */

	return key;
}

void writekey(const char* filename, const struct rsakey* key)	/* write key to file */
{
	FILE* file = fopen(filename, "w");		/* open file */
	if (file == NULL) pexit("fopen");		/* if not readed exit */
	fprintf(file, "%lld:%lld\n", key->a, key->b);	/* write key to file */
	fclose(file);					/* close file */
}

long long int applykey(const struct rsakey* key, long long int c)	/* apply key to value */
{									/* fast approximation of */
        int i;								/* (c ^ key->a) % key-> b */
	unsigned long long int res = 1;
        for (i = 0; i < key->a; i++) res = (res * c) % key->b;
        return res;
}
