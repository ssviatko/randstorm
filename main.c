#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <time.h>

#include "sha2.h"

#define STR_MAX 64

unsigned char randbuff[1048576]; // 1 MB buffer
unsigned char hash[SHA512_DIGEST_SIZE];

static void usage(const char *a_prog)
{
	fprintf(stderr, "usage: %s -n <megabytes to generate> -s <passphrase, max 64 characters>\n", a_prog);
}

int main(int argc, char **argv)
{
	int opt;
	int have_n = 0;
	int have_s = 0;
	long n = 0;
	char *endp;
	char str[STR_MAX + 1] = "";

	/* program name + "-n" value + "-s" value */
	if (argc < 5) {
		usage(argv[0]);
		return 1;
	}

	while ((opt = getopt(argc, argv, "n:s:")) != -1) {
		switch (opt) {
			case 's':
				if (strlen(optarg) > STR_MAX) {
					fprintf(stderr, "%s: passphrase too long (max %d characters)\n", argv[0], STR_MAX);
					return 1;
				}
				strcpy(str, optarg);
				have_s = 1;
				break;
			case 'n':
				errno = 0;
				n = strtol(optarg, &endp, 10);
				if (errno != 0 || endp == optarg || *endp != '\0' || n < INT_MIN || n > INT_MAX) {
					fprintf(stderr, "%s: invalid integer '%s'\n", argv[0], optarg);
					return 1;
				}
				have_n = 1;
				break;
			default:
				usage(argv[0]);
				return 1;
		}
	}

	if (!have_n) {
		fprintf(stderr, "%s: -n is required\n", argv[0]);
		usage(argv[0]);
		return 1;
	}

	if (!have_s) {
		fprintf(stderr, "%s: -s is required\n", argv[0]);
		usage(argv[0]);
		return 1;
	}

	printf("megabytes = %ld\n", n);
	printf("passphrase = '%s'\n", str);

	struct timespec ts_start, ts_end;
	long long elapsed_us;

	// time start
	clock_gettime(CLOCK_MONOTONIC, &ts_start);

    // seed hash with passphrase
    sha512((const unsigned char *)str, strlen(str), hash);

	printf("seeded hash from passphrase = ");
	for (int i = 0; i < SHA512_DIGEST_SIZE; ++i)
		printf("%02x", hash[i]);
	printf("\n");

    for (long i = 0; i < n; ++i) {
        for (long j = 0; j < 1048576; j += SHA512_DIGEST_SIZE) {
            sha512(hash, SHA512_DIGEST_SIZE, hash);
            memcpy(randbuff + j, hash, SHA512_DIGEST_SIZE);
        }
    }

 	// time end
	clock_gettime(CLOCK_MONOTONIC, &ts_end);

	elapsed_us = (long long)(ts_end.tv_sec - ts_start.tv_sec) * 1000000LL
		+ (ts_end.tv_nsec - ts_start.tv_nsec) / 1000;
	printf("time used: %lld.%06lld seconds (%lld us)\n", elapsed_us / 1000000, elapsed_us % 1000000, elapsed_us);

	return 0;
}
