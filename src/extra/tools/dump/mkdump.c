/*
:*:	convert binary file in ascii representation
:de: 	Generieren eines dumps

$Copyright (C) 1991 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define LEN	60

/*
#define	AUFRUF	"Aufruf: %s Ein Aus\n"
#define	NOREAD	"%s: File %s nicht lesbar.\n"
#define	NOWRITE	"%s: File %s nicht beschreibbar.\n"
*/
#define	AUFRUF	"usage: %s in out\n"
#define	NOREAD	"%s: file %s not readable.\n"
#define	NOWRITE	"%s: file %s not writeable.\n"

/*
$pconfig
:in |
	:*:input file
	:de:Eingabefile
:out |
	:*:output file
	:de:Ausgabefile
*/

static void usage (const char *name, const char *arg)
{
	execlp("efeuman", "efeuman", "--", __FILE__, arg, NULL);
	fprintf(stderr, AUFRUF, name);
}

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

	if	(narg > 1)
	{
		if	(strcmp("-?", arg[1]) == 0)
			usage(arg[0], arg[1]);
		if	(strncmp("--help", arg[1], 6) == 0)
			usage(arg[0], arg[1]);
	}

	if	(narg != 3)
	{
		usage(arg[0], "-?");
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
