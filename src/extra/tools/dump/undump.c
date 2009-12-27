/*
:*:	restore binary file from ascii representation
:de:	Ruecktransformation eines dump-Files

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

/*
#define	AUFRUF	"Aufruf: %s Ein Aus\n"
#define	NOREAD	"%s: File %s nicht lesbar.\n"
#define	NOWRITE	"%s: File %s nicht beschreibbar.\n"
*/
#define	AUFRUF	"usage: %s in out\n"
#define	NOREAD	"%s: file %s not readable.\n"
#define	NOWRITE	"%s: file %s not writeable.\n"


/*	Lesen eines Zeichens
*/

static int getdump (FILE *ein)
{
	int c;		/* gelesenes Zeichen */
	int k;		/* Hilfsvariable */

	c = getc(ein);

	if (c == EOF || c == 0)	return(c);

/*	Test auf 8-Bit Flag
*/
	if	(c == '~')
	{
		c = getc(ein) & 0x7f;
		k = 0x80;
	}
	else	k = 0;

/*	Steuerzeichen
*/
	if	(c == '^')
	{
		c = getc(ein);

		if (c == '?')	return (k | 0x7f);
		else		return (k | (c & 0x1f));
	}

/*	Sonderzeichen
*/
	if	(c == '\\')
	{
		switch (c = getc(ein))
		{
		case '\n':	c = getdump(ein); break;
		case 'd':	c = 0x7f; break;
		case 'n':	c = '\n'; break;
		case 'r':	c = '\r'; break;
		case 'f':	c = '\f'; break;
		case 't':	c = '\t'; break;
		case 'b':	c = '\b'; break;
		case 'E':
		case 'e':	c = 0x1b; break;
		case 'X':
		case 'x':	fscanf(ein, "%2x", &c); break;
		case '0':	fscanf(ein, "%3o", &c); break;
		default:	break;
		}
	}

	return (k | c);
}


/*	Hauptprogramm
*/

int main (int narg, char **arg)
{
	FILE *ein;	/* Eingabefile */
	FILE *aus;	/* Ausgabefile */

	int c;		/* gelesenes Zeichen */

	if	(narg != 3)
	{
		fprintf(stderr, AUFRUF, arg[0]);
		return 1;
	}

	if	(strcmp(arg[1], "-") == 0)
	{
		ein = stdin;
	}
	else if	(!(ein = fopen(arg[1], "r")))
	{
		fprintf(stderr, NOREAD, arg[0], arg[1]);
		return 1;
	}

	if	(strcmp(arg[2], "-") == 0)
	{
		aus = stdout;
	}
	else if	(!(aus = fopen(arg[2], "wb")))
	{
		fprintf(stderr, NOWRITE, arg[0], arg[2]);
		return 1;
	}

	while ((c = getdump(ein)) != EOF)
		putc(c, aus);

	return 0;
}
