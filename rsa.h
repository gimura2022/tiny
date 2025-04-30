#ifndef _rsa_h	/* rsa - tiny rsa realisation */
#define _rsa_h

#include <stddef.h>

struct rsakey { long long int a, b; };	/* rsa key structure */

struct rsakey readkey(const char* filename);
void writekey(const char* filename, const struct rsakey* key);				
long long int applykey(const struct rsakey* key, long long int c);

#endif
