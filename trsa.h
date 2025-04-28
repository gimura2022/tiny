#ifndef _trsa_h	/* trsa - tiny rsa realisation */
#define _trsa_h

#include <stddef.h>

struct rsakey { int a, b; };	/* rsa key structure */

struct rsakey readkey(const char* filename);
void writekey(const char* filename, const struct rsakey* key);				
unsigned long long int applykey(const struct rsakey* key, unsigned long long int c);

#endif
