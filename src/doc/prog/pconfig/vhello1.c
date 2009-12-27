#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv)
{
	char *fmt = "%s";
	char *label = "Hello World!";
	int opt;

	while ((opt = getopt(argc, argv, "qf:")) != -1)
	{
		switch (opt)
		{
		case 'q':
			fmt = "\"%s\"";
			break;
		case 'f':
			fmt = optarg;
			break;
		default:
			exit(1);
		}
	}

	if	(optind < argc)	label = argv[optind];

	printf(fmt, label);
	putchar('\n');
	exit(0);
}
