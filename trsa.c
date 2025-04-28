#include <stdio.h>	/* trsa - tiny rsa realisation */
#include <stdlib.h>	/* you need to have there headers in your system to compile tiny */
#include <err.h>

#include "trsa.h"	/* tiny rsa header */

#define pexit(x) { perror(x); exit(-1); }	/* for print the error from a errno and exit */

struct rsakey readkey(const char* filename)	/* read key from file */
{
	struct rsakey key;			/* output key */

	FILE* file = fopen(filename, "r");	/* read file */
	if (file == NULL) pexit("fopen");
	if (fscanf(file, "%d:%d", &key.a, &key.b) != 2) err(EXIT_FAILURE, "invalid keyfile format");
	fclose(file);				/* close file */

	return key;
}

void writekey(const char* filename, const struct rsakey* key)	/* write key to file */
{
	FILE* file = fopen(filename, "w");	/* open file */
	if (file == NULL) pexit("fopen");	/* if not readed exit */
	fprintf(file, "%d:%d", key->a, key->b);	/* write key to file */
	fclose(file);				/* close file */
}

unsigned long long int applykey(const struct rsakey* key, unsigned long long int c)	/* apply key to */
{											/* value */
        int i;
        unsigned long long int res = 1;

        for (i = 0; i < key->a; i++) res = (res * c) % key->b;

        return res;
}
