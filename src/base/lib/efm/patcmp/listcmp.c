/*
Vergleich eines Zeichens mit einer Liste

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/patcmp.h>
#include <ctype.h>


/*	Metazeichen
*/

#define	ESCAPE(x)	(x == '\\')
#define	RANGE(x)	(x == '-')
#define	NEGATION(x)	(x == '!' || x == '^')
#define	SPECIAL(x)	(x == '%')

#define	ODIGIT(c)	(isdigit(c) && c != '8' && c != '9')


/*	Vergleich mit Zeichenliste
*/

int listcmp(const char *ptr, int c)
{
	const uchar_t *list;	/* Liste */
	int flag;		/* Negationsflag */
	int n;			/* Hilfszähler */

/*	Test auf leere Liste
*/
	if	(ptr == NULL)	return 0;

	list = (const uchar_t *) ptr;
	c = (uchar_t) c;
	flag = 1;

/*	Test auf Negationszeichen
*/
	while (NEGATION(*list))
	{
		list++;
		flag = ! flag;
	}

/*	Abarbeiten der Vergleichsliste
*/
	for (n = 0; list[n] != 0; n++)
	{
		if	(list[n + 1] == 0)
		{
			if	(c == list[n])
				return flag;
		}
		else if	(SPECIAL(list[n]))
		{
			int f;

			n++;

			switch (list[n])
			{
			case 's':	f = isspace(c); break;
			case 'p':	f = ispunct(c); break;
			case 'd':	f = isdigit(c); break;
			case 'o':	f = ODIGIT(c); break;
			case 'x':	f = isxdigit(c); break;
			case 'u':	f = isupper(c); break;
			case 'l':	f = islower(c); break;
			case 'a':	f = isalpha(c); break;
			case 'n':	f = isalnum(c); break;
			case '@':	f = (c == 0); break;
			case '^':	f = iscntrl(c); break;
			case '~':	f = (c & 0x80); break;
			default:	f = (c == list[n]); break;
			}

			if	(f)	return flag;
		}
		else if	(RANGE(list[n]) && n > 0)
		{
			int a, b;

			a = list[n-1];
			n++;

			if	(ESCAPE(list[n]) && list[n+1] != 0)
				n++;

			if	(a > list[n])
			{
				b = a;
				a = list[n];
			}
			else	b = list[n];

			if	(a <= c && c <= b)
				return flag;
		}
		else
		{
			if (ESCAPE(list[n]))	n++;
			if (c == list[n])	return flag;
		}
	}

	return (!flag);
}
