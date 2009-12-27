/*	Druckerfilter
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

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
		case 'ä':	put_german('{'); break;
		case 'ö':	put_german('|'); break;
		case 'ü':	put_german('}'); break;
		case 'Ä':	put_german('['); break;
		case 'Ö':	put_german('\\'); break;
		case 'Ü':	put_german(']'); break;
		case 'ß':	put_german('~'); break;
		default:	putchar(c); break;
		}
	}

	return 0;
}
