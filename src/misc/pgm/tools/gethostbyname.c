#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

static void show_addr(unsigned char *p, size_t len)
{
	char *delim = " ";

	while (len-- > 0)
	{
		fputs(delim, stdout);
		delim = ".";
		printf("%d", (unsigned char) *(p++));
	}
}

int main(int narg, char **arg)
{
	struct hostent *host;
	char **p;
	int i;

	for (i = 1; i < narg; i++)
	{
		printf("%s: ", arg[i]);
		host = gethostbyname(arg[i]);

		if	(host)
		{
			printf("%s", host->h_name);

			for (p = host->h_addr_list; *p != NULL; p++)
				show_addr(*p, host->h_length);

			putchar('\n');
		}
		else	fputs("nicht gefunden\n", stdout);
	}

	return 0;
}
