#include <stdint.h>	/* rsak - tiny rsa key generator */
#include <stdio.h>	/* you need to have there headers in your system to compile tiny */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "trsa.h"	/* tiny rsa header */

static bool is_prime(uint16_t x)	/* check to number is prime */
{
	uint16_t i;

	for (i = 2; i <= x / 2; i++)
		if (x % i == 0)
			return false;

	return true;
}

static uint16_t get_prime(void)	/* get random prime number */
{
	uint16_t res;

	do {
		srand(time(NULL));
		res = rand() % UINT16_MAX + 5;
	} while (!is_prime(res));

	return res;
}

uint32_t find_d(uint16_t e, uint32_t phi)	/* find constant for private key */
{
	uint32_t eprev, dprev, d = 1, etemp, dtemp;
	eprev = phi, dprev = phi;

	while (e != 1) {
		etemp = e;
		dtemp = d;
		e = eprev - eprev / etemp * e;
		d = dprev - eprev / etemp * d;
		eprev = etemp;
		dprev = dtemp;
		while (d < 0) d += phi;
	}

	return d;
}

int main(int argc, char* argv[])
{
	char buf[1024];			/* buffer used for filename storing */
	uint16_t p, q;			/* random prime numbers */
	struct rsakey pubkey, privkey;	/* public and private key */

	if (argc < 2 || strcmp(argv[1], "-h") == 0) {				 /* check arguments */
		puts("usage: rsak keyfile [-h]\n""	-h	print usage\n"); /* if format invalid or */
		exit(argc < 2 ? -2 : EXIT_SUCCESS);				 /* given -h, print usage */
	}

	p = get_prime();	/* generate 2 prime numbers */
	q = get_prime();

	pubkey  = (struct rsakey) { 3, p * q };					/* generate keys */
	privkey = (struct rsakey) { find_d(3, (p - 1) * (q - 1)), p * q };

	snprintf(buf, sizeof(buf), "%s.trsapub", argv[1]);	/* get public key file name */
	writekey(buf, &pubkey);					/* write key to file */
	snprintf(buf, sizeof(buf), "%s.trsapriv", argv[1]);	/* get private key file name */
	writekey(buf, &privkey);				/* write key to file */

	return 0;
}
