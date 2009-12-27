/*
IO-Strukturen umkopieren

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

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


int iocpy_flag = 0;	/* Flag zur Kennung von Sonderzeichen */
int iocpy_last = 0;	/* Zuletzt gelesenes Zeichen */

#define	CSTAT_STD	0	/* Standardzeichen */
#define	CSTAT_BREAK	1	/* Abbruchzeichen */
#define	CSTAT_SPEC	2	/* Spezielle Kopierroutine */


/*	Substitutionsbefehle interpretieren
*/

int iocpy(io_t *in, io_t *out, iocpy_t *def, size_t dim)
{
	int n, i, k, c;
	int stat;
	int flag;

	flag = 0;
	n = 0;

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		stat = CSTAT_STD;

	/*	Suche nach einer speziellen Kopiervorschrift
	*/
		for (i = 0; i < dim; i++)
		{
			if	(!def[i].key || listcmp(def[i].key, c))
			{
				if	(def[i].func == NULL)
				{
					if	(def[i].flags)
					{
						if (io_putc(c, out) != EOF)
							n++;

						c = EOF;
					}

					stat = CSTAT_BREAK;
					break;
				}

				k = def[i].func(in, out, c, def[i].arg,
					def[i].flags);

				if	(k != EOF)
				{
					n += k;
					stat = CSTAT_SPEC;
					flag = 1;
					break;
				}
			}
		}

	/*	Status nach der Suche auswerten
	*/
		if	(stat == CSTAT_STD)
		{
			if	(io_putc(c, out) != EOF)	n++;
		}
		else if	(stat == CSTAT_BREAK)
		{
			break;
		}
	}

	io_ungetc(c, in);
	iocpy_last = c;
	iocpy_flag = flag;
	return n;
}


int iocpyfmt(const char *fmt, io_t *out, iocpy_t *def, size_t dim)
{
	io_t *in;
	int n;

	in = io_cstr(fmt);
	n = iocpy(in, out, def, dim);
	io_close(in);
	return n;
}
