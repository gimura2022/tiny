/*
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted.

 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* tiny - simple serial two side messenger */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* size for buffers inside code */
#define BUFFER_SIZE 1024

#define NET_FAILURE 0	/* exit code for net errors */
#define ARG_FAILURE 1	/* exit code for invalid arguments */

#define get_bit(n, i) (((n) >> (i)) & 1)	/* get i bit from n */
#define set_bit(n, i, x) ((x) == 0 ? \
		((n) & (~(1 << ((i) - 1)))) : \
		((n) | (1 << ((i) - 1))))	/* set i bin from n */

#define array_len(x) (sizeof(x) / sizeof((x)[0]))	/* get array lenght */

/* encryption type */
enum encrypt {
	ENCRYPT_NONE = 0,
	ENCRYPT_XOR,
};

static uint16_t port        = 8080;		/* net port */
static const char* address  = NULL;		/* ip address or domain */
static enum encrypt encrypt = ENCRYPT_NONE;	/* encryption method (default no encryption) */
static uint32_t encrypt_key = 0;		/* key for encryption */
static bool verbose_mode    = false;		/* verbose mode */

/* verbose print */
static void verbosef(const char* fmt, ...)
{
	va_list args;

	if (!verbose_mode)
		return;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

/* table for mixing bits in encrypt key */
static uint8_t mix_encrypt_key_table[] = {
	29, 26, 30, 15, 20, 12, 13, 16,
	22, 11,  9, 17,  8, 27, 24,  4,
	10, 14, 25, 21,  1,  6, 31,  2,
	 7, 18, 28, 23, 32, 19,  5,  3
};

/* mix encrypt key bits */
static uint32_t mix_encrypt_key(uint32_t old_key)
{
	uint32_t res;
	int i;

	for (i = 0; i < array_len(mix_encrypt_key_table); i++)
		res = set_bit(res, i, get_bit(old_key, mix_encrypt_key_table[i]));

	verbosef("setuped new encryption key: %u\n", res);

	return res;
}

/* encrypt line by xor method */
static uint8_t* encrypt_xor(const char* source, size_t* out_len)
{
	uint32_t* out;
	int i;

	out      = malloc(strlen(source) * sizeof(uint32_t));
	*out_len = strlen(source) * sizeof(uint32_t);

	out[0] = source[0] ^ encrypt_key;
	for (i = 1; i < strlen(source); i++)
		out[i] = source[i] ^ source[i - 1];

	verbosef("encrypted messange: ");
	for (i = 0; i < *out_len; i++)
		verbosef("%x", ((uint8_t*) out)[i]);
	verbosef("\n");
	verbosef("lenght: %u\n", *out_len);

	return (uint8_t*) out;
}

/* decrypt line by xor method */
static char* decrypt_xor(const uint8_t* source, size_t len)
{
	char* out;
	uint32_t* int_source;
	int i;

	verbosef("encrypted messange length: %u\n", len);

	if (len % sizeof(uint32_t) != 0) {
		fprintf(stderr, "invalid encryption\n");
		exit(NET_FAILURE);
	}

	int_source                  = (uint32_t*) source;
	out                         = malloc((len / sizeof(uint32_t)) + 1);
	out[len / sizeof(uint32_t)] = '\0';

	out[0] = int_source[0] ^ encrypt_key;
	for (i = 1; i < len / sizeof(uint32_t); i++)
		out[i] = int_source[i] ^ out[i - 1];

	return out;
}

/* encrypt text to encoded bytes */
static uint8_t* encrypt_msg(const char* source, size_t* out_len)
{
	switch (encrypt) {
	case ENCRYPT_NONE:
		*out_len = strlen(source);
		return memcpy(malloc(strlen(source)), source, strlen(source));

	case ENCRYPT_XOR:
		return encrypt_xor(source, out_len);
	}
}

/* decrypt encoded bytes to text */
static char* decrypt_msg(const uint8_t* source, size_t len)
{
	switch (encrypt) {
	case ENCRYPT_NONE:
		return memcpy(malloc(len), source, len);

	case ENCRYPT_XOR:
		return decrypt_xor(source, len);
	}
}

/* start tiny at host mode */
static void host(void)
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFFER_SIZE] = {0};
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(NET_FAILURE);
	}
	
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(NET_FAILURE);
	}
	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(NET_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(NET_FAILURE);
	}

	printf("server listen at %u...", port);
	fflush(stdout);
	
	if ((new_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen)) < 0) {
		perror("accept");
		exit(NET_FAILURE);
	}
	
	printf("connected\n");
	
	while(1) {
		verbosef("wait for otherside responce\n");
		int valread = read(new_socket, buffer, BUFFER_SIZE);

		if (valread <= 0) {
			printf("disconect\n");
			break;
		}

		char* decrypted = decrypt_msg((uint8_t*) buffer, valread);
		printf("otherside: %s\n", decrypted);
		free(decrypted);
		
		printf("you: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		buffer[strlen(buffer) - 1] = '\0';

		size_t out_len;
		uint8_t* encrypted = encrypt_msg(buffer, &out_len);
		send(new_socket, encrypted, out_len, 0);
		free(encrypted);
	}
	
	close(new_socket);
	close(server_fd);
}

/* start tiny at client mode */
static void client(void)
{
	int sock;
	struct sockaddr_in serv_addr;
	char buffer[BUFFER_SIZE] = {0};
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(NET_FAILURE);
	}
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	
	if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
		perror("inet_pton");
		exit(NET_FAILURE);
	}

	printf("connect to server at %s:%u...", address, port);
	fflush(stdout);
	
	if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect");
		exit(NET_FAILURE);
	}
	
	printf("success\n");
	
	while(1) {
		printf("you: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		buffer[strlen(buffer) - 1] = '\0';

		size_t out_len;
		uint8_t* encrypted = encrypt_msg(buffer, &out_len);
		send(sock, encrypted, out_len, 0);
		free(encrypted);
		
		verbosef("wait for otherside responce\n");
		int valread = read(sock, buffer, BUFFER_SIZE);

		if (valread <= 0) {
			printf("disconnect\n");
			break;
		}

		char* decrypted = decrypt_msg((uint8_t*) buffer, valread);
		printf("otherside: %s\n", decrypted);
		free(decrypted);
	}
	
	close(sock);
}

#define USAGE_SMALL "usage: tiny [-h] [-H] [-c] [-p port] [-a address] [-e none xor] [-k number] [-v]\n"
#define USAGE \
	"	-h	to print usage\n" \
	"	-H	to start tiny in host mode\n" \
	"	-c	to start tiny in client mode\n" \
	"	-p	port\n" \
	"	-a	address\n" \
	"	-e	select one of:\n" \
	"			none	without encryption\n" \
	"			xor	xor encryption\n" \
	"	-k	key for encryption\n" \
	"	-v	enable verbose mode\n"

/* print usage */
static void usage(FILE* stream, bool small)
{
	fprintf(stream, small ? USAGE_SMALL : USAGE_SMALL USAGE);
}

/* get encryption method from string */
static enum encrypt get_encryption_method(const char* str)
{
	if (strcmp(str, "none") == 0)
		return ENCRYPT_NONE;
	else if (strcmp(str, "xor") == 0)
		return ENCRYPT_XOR;

	fprintf(stderr, "tiny: invalid encryption method name\n");
	usage(stderr, true);
	exit(ARG_FAILURE);
}

int main(int argc, char* argv[])
{
	enum {
		MODE_UNSPECIFIED = 0,
		MODE_HOST,
		MODE_CLIENT,
	} mode = MODE_UNSPECIFIED;

	int c;
	while ((c = getopt(argc, argv, "Hhcp:a:e:k:v")) != -1) switch (c) {
	case 'H':
		mode = MODE_HOST;
		break;

	case 'c':
		mode = MODE_CLIENT;
		break;

	case 'p':
		port = atoi(optarg);
		break;

	case 'a':
		address = optarg;
		break;

	case 'e':
		encrypt = get_encryption_method(optarg);
		break;

	case 'k':
		encrypt_key = mix_encrypt_key(atoi(optarg));
		break;

	case 'h':
		usage(stdout, false);
		exit(EXIT_SUCCESS);

	case 'v':
		verbose_mode = true;
		break;

	case '?':
		fprintf(stderr, "tiny: invalid option\n");
		usage(stderr, true);
		exit(ARG_FAILURE);
	}

	switch (mode) {
	case MODE_CLIENT:
		if (address == NULL) {
			fprintf(stderr, "tiny: host address not specified\n");
			usage(stderr, true);
			exit(ARG_FAILURE);
		}

		client();

		break;

	case MODE_HOST:
		host();
		break;

	case MODE_UNSPECIFIED:
		fprintf(stderr, "tiny: working mode not specified\n");
		usage(stderr, true);
		exit(ARG_FAILURE);
	}

	return 0;
}
