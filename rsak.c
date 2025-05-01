#include <stdint.h>	/* rsak - tiny rsa key generator */
#include <stdio.h>	/* you need to have libc, posix and rsa.h headers */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "rsa.h"	/* tiny rsa header */

#define E 257

static bool is_prime(uint16_t x)	/* check to number is prime */
{
	uint16_t i;
	for (i = 2; i <= x / 2; i++) if (x % i == 0) return false;
	return true;
}

static unsigned long long int get_prime(void)	/* get random prime number */
{
	uint16_t res;
	do res = rand() % UINT16_MAX + 5; while (!is_prime(res));
	return res;
}

static long long int gcde(long long int a, long long int b, long long int* x, long long int* y)	/* for d */
{
	if (!a) {
		*x = 0, *y = 1;
		return b;
	}

	long long int x1, y1;
	long long int gcd = gcde(b % a, a, &x1, &y1);
	*x = y1 - (b / a) * x1;
	*y = x1;
	return gcd;
}

int main(int argc, char* argv[])
{
	char buf[1024];			/* buffer used for filename storing */
	long long int p, q, f, d, y;	/* random prime numbers */
	struct rsakey pubkey, privkey;	/* public and private key */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				 /* check arguments */
		puts("usage: rsak keyfile [-h]\n""	-h	print usage"); /* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				 /* given -h, print usage */
	}

	srand(time(NULL));	/* set random seed */

	p = get_prime(), q = get_prime();	/* generate 2 prime numbers */
	f = (p - 1) * (q - 1);			/* find f(n) = (p - 1) * (q - 1) */
	d = gcde(E, f, &d, &y) < 0 ? d + f : d;	/* find private exponent */

	pubkey = (struct rsakey) { E, p * q }, privkey = (struct rsakey) { d, p * q }; /* keys */

	snprintf(buf, sizeof(buf), "%s.trsapub", argv[1]);	/* get public key file name */
	writekey(buf, &pubkey);					/* write key to file */
	snprintf(buf, sizeof(buf), "%s.trsapriv", argv[1]);	/* get private key file name */
	writekey(buf, &privkey);				/* write key to file */

	return 0;
}
