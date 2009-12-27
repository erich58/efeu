/*	Generieren eines dumps
	(c) 1991 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/

#include <stdio.h>

#define LEN	60

#define	AUFRUF	"Aufruf: %s Ein Aus\n"
#define	NOREAD	"%s: File %s nicht lesbar.\n"
#define	NOWRITE	"%s: File %s nicht beschreibbar.\n"


/*	Ausgabe eines Zeichens
*/

static int putdump(int c, FILE *aus)
{
	int n;

/*	Zeichen, wo 8. Bit gesetzt ist
*/
	if	(c & ~0x7f)
	{
		putc('~', aus);
		c &= 0x7f;
		n = 1;

		switch (c)
		{
		case '\n':
			putc('^', aus);
			putc(c | 0x40, aus);
			return 3;
		case '~':
			putc(c, aus);
			return 2;
		default:
			break;
		}
	}
	else	n = 0;

/*	Zeichen mit Sonderbehandlung
*/
	switch (c)
	{
	case '\n':
		putc('\n', aus);
		return 0;
	case '~':
	case '^':
	case '\\':
		putc('\\', aus);
		putc(c, aus);
		return (n + 2);
	case 127:
		putc('^', aus);
		putc('?', aus);
		return (n + 2);
	default:
		break;
	}

/*	Sonstige Zeichen
*/
	if	(c & ~0x1f)
	{
		putc(c, aus);
		return (n + 1);
	}
	else
	{
		putc('^', aus);
		putc(c | 0x40, aus);
		return (n + 2);
	}
}


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	FILE *ein;	/* Eingabefile */
	FILE *aus;	/* Ausgabefile */

	int c;		/* gelesenes Zeichen */
	int k = 0;	/* Laenge der aktuellen Zeile */

	if	(narg != 3)
	{
		fprintf(stderr, AUFRUF, arg[0]);
		return 1;
	}

	if	(strcmp(arg[1], "-") == 0)
	{
		ein = stdin;
	}
	else if	(!(ein = fopen(arg[1], "rb")))
	{
		fprintf(stderr, NOREAD, arg[0], arg[1]);
		return 1;
	}

	if	(strcmp(arg[2], "-") == 0)
	{
		aus = stdout;
	}
	else if	(!(aus = fopen(arg[2], "w")))
	{
		fprintf(stderr, NOWRITE, arg[0], arg[2]);
		return 1;
	}

	while ((c = getc(ein)) != EOF)
	{
		if	(k >= LEN && c != '\n')
		{
			putc('\\', aus);
			putc('\n', aus);
			k = 0;
		}

		k += putdump(c, aus);

		if	(c == '\n')	k = 0;
	}

	if	(k)
	{
		putc('\\', aus);
		putc('\n', aus);
	}

	return 0;
}
