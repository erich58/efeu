#include <EFEU/Resource.h>
#include <EFEU/vecbuf.h>

static int comp1 (const void *a_p, const void *b_p)
{
	char const * const *a = a_p;
	char const * const *b = b_p;
	return strcmp(*a, *b);
}

static int comp2 (const void *a_p, const void *b_p)
{
	char const *a = a_p;
	char const * const *b = b_p;
	return a ? strncmp(a, *b, strlen(a)) : 0;
}

int main (int argc, char **argv)
{
	int n;
	char *key;
	VecMatch match;

	ParseCommand(&argc, argv);
	argc--;
	argv++;

	if	(argc > 1)
		qsort(argv, argc, sizeof argv[0], comp1);

	key = GetResource("Key", NULL);
	match = vmatch(key, argv, argc, sizeof argv[0], comp2);

	for (n = 0; n < argc; n++)
	{
		printf("%2d: ", n);

		if	(n < match.start)	fputs("< ", stdout);
		else if	(n < match.end)		fputs("= ", stdout);
		else				fputs("> ", stdout);

		printf("%s\n", argv[n]);
	}

	return 0;
}

