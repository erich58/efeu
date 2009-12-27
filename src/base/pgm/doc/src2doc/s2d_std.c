/*
Standardverarbeitung

$Copyright (C) 2000 Erich Frühstück
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

#include "src2doc.h"
#include <EFEU/strbuf.h>
#include <EFEU/patcmp.h>
#include <EFEU/parsub.h>
#include <EFEU/ftools.h>
#include <ctype.h>

/*
#define	BEG "\n\\spage[3]\n\n---- verbatim\n"
#define	END "----\n\\end\n"
*/
#define	BEG "\n---- verbatim\n"
#define	END "----\n\n"

static void copyline (IO *ein, IO *aus)
{
	int c;

	while ((c = io_getc(ein)) != EOF && c != '\n')
		io_putc(c, aus);

	io_putc('\n', aus);
}


static void subcopy (const char *name, IO *ein, IO *aus, IO *src)
{
	StrBuf *buf;
	int c, flag, nlcount;

	buf = new_strbuf(0);
	flag = 0;
	nlcount = 0;

/*	Das erste Kommentar wird gesondert behandelt
*/
	c = io_skipcom(ein, buf, 1);

	if	(sb_getpos(buf))
	{
		char *p;

		sb_putc(0, buf);

		for (p = (char *) buf->data; *p != 0; p++)
		{
			if	(*p == '\n')
			{
				p[1] = 0;
				break;
			}
		}

		io_psubvec(aus, (char *) buf->data, 0, NULL);
		sb_clear(buf);
	}
	else if	(name)
	{
		io_printf(aus, "\\title\t%s\n\n");
	}

	do
	{
		if	(sb_getpos(buf))
		{
			if	(flag)
				io_puts(END, src);

			sb_putc(0, buf);
			io_putc('\n', aus);
			io_psubvec(aus, (char *) buf->data, 0, NULL);
			sb_clear(buf);
			flag = 0;
		}

		if	(c != '\n')
		{
			if	(!flag)
			{
				io_puts(BEG, src);
				flag = 1;
				nlcount = 0;
			}

			io_nputc('\n', src, nlcount);
			nlcount = 0;
			io_putc(c, src);
			copyline(ein, src);
		}
		else	nlcount++;
	}
	while ((c = io_skipcom(ein, buf, 1)) != EOF);

	if	(flag)
		io_puts(END, src);

	del_strbuf(buf);
}

void s2d_std (const char *name, IO *ein, IO *aus)
{
	subcopy(name, ein, aus, aus);
}

void s2d_com (const char *name, IO *ein, IO *aus)
{
	subcopy(name, ein, aus, NULL);
}

void s2d_doc (const char *name, IO *ein, IO *aus)
{
	io_copy(ein, aus);
}

void s2d_man (const char *name, IO *ein, IO *aus)
{
	char *fname, *suffix;

	fname = mbasename(name, &suffix);

	if	(fname)
	{
		io_printf(aus, "\\mpage[%s] %s\n",
			Secnum ? Secnum : suffix, fname);
		memfree(fname);
	}

	io_copy(ein, aus);
}
