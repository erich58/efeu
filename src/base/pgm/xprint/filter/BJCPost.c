/*	Druckerfilter
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 3.0
*/

#include <stdio.h>

#define	ESC	033

static void put_german(int c)
{
	printf("%cR%c%c%cR%c", ESC, 2, c, ESC, 0);
}

int main(int narg, char **arg)
{
	int c;

	while ((c = getchar()) != EOF)
	{
		switch ((char) c)
		{
		case '�':	put_german('{'); break;
		case '�':	put_german('|'); break;
		case '�':	put_german('}'); break;
		case '�':	put_german('['); break;
		case '�':	put_german('\\'); break;
		case '�':	put_german(']'); break;
		case '�':	put_german('~'); break;
		default:	putchar(c); break;
		}
	}

	return 0;
}
