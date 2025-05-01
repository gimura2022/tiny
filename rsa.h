#ifndef _rsa_h	/* rsa - tiny rsa header */
#define _rsa_h	/* you need to have rsa.c source file */

#include <stddef.h>

struct rsakey { long long int a, b; };					/* rsa key structure */

struct rsakey readkey(const char* filename);				/* read key from file */
void writekey(const char* filename, const struct rsakey* key);		/* write key to file */
long long int applykey(const struct rsakey* key, long long int c);	/* apply key to number */

#endif
