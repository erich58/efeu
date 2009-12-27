/*
Creating Pascal's triangle
*/

#include <stdlib.h>
#include <stdio.h>
#include "binom.h"

int main (int argc, char **argv)
{
	int n, i, j;
	char *fmt = "%5d";

	if	(argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: %s n [fmt]\n", argv[0]);
		return EXIT_FAILURE;
	}

	n = atoi(argv[1]);

	if	(argc > 2)
		fmt = argv[2];

	if	(n < 0)
	{
		fprintf(stderr, "%s: n must be positive.\n", argv[0]);
		return EXIT_FAILURE;
	}

	if	(n > 12)
	{
		fprintf(stderr, "%s: can't handle n > 12.\n", argv[0]);
		return EXIT_FAILURE;
	}

	for (i = 0; i <= n; i++)
	{
		for (j = 0; j <= i; j++)
		{
			printf(fmt, bincoef(i, j));
		}

		printf("\n");
	}

	return EXIT_SUCCESS;
}
