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


static int (*skipcom)(IO *ein, StrBuf *buf, int flag) = io_skipcom;

static void comment_line (IO *io, StrBuf *buf)
{
	int c, escape;

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	escape = 0;

	while (escape || c != '\n')
	{
		if	(c == EOF)	return;
		if	(c == '\n')	io_linemark(io);

		if	(buf)	sb_putc(c, buf);

		escape = (c == '\\') ? !escape : 0;
		c = io_getc(io);
	}
	
	if	(buf)	sb_putc(c, buf);
}

static int skipcom_script (IO *io, StrBuf *buf, int flag)
{
	int c;

	while ((c = io_getc(io)) == '#')
		comment_line(io, buf);

	return c;
}

/*	Dateikopf aus erstem Kommentar bestimmen
*/

static void std_head (const char *name, StrBuf *buf, IO *ein, IO *aus)
{
	int c = skipcom(ein, buf, 1);

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

		io_psubarg(aus, (char *) buf->data, "ns", name);
		sb_clean(buf);
	}
	else if	(name)
	{
		io_xprintf(aus, "\\title\t%s\n\n", name);
	}

	io_ungetc(c, ein);
}

static void std_copy (const char *name, StrBuf *buf, IO *ein, IO *aus, IO *src)
{
	int c, flag, nlcount;

	flag = 0;
	nlcount = 0;

	while ((c = skipcom(ein, buf, 1)) != EOF)
	{
		if	(sb_getpos(buf))
		{
			if	(flag)
				io_puts(END, src);

			sb_putc(0, buf);
			io_putc('\n', aus);
			io_psubarg(aus, (char *) buf->data, "ns", name);
			sb_clean(buf);
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

	if	(flag)
		io_puts(END, src);

	if	(sb_getpos(buf))
	{
		sb_putc(0, buf);
		io_putc('\n', aus);
		io_psubarg(aus, (char *) buf->data, "ns", name);
		sb_clean(buf);
	}
}

void s2d_std (const char *name, IO *ein, IO *aus)
{
	StrBuf *buf = sb_acquire();
	std_head(name, buf, ein, aus);
	std_copy(name, buf, ein, aus, aus);
	sb_release(buf);
}

void s2d_xstd (const char *name, IO *ein, IO *aus)
{
	StrBuf *buf = sb_acquire();
	std_copy(name, buf, ein, aus, aus);
	sb_release(buf);
}

void s2d_script (const char *name, IO *ein, IO *aus)
{
	skipcom = skipcom_script;
	s2d_std(name, ein, aus);
}

void s2d_xscript (const char *name, IO *ein, IO *aus)
{
	skipcom = skipcom_script;
	s2d_xstd(name, ein, aus);
}

void s2d_com (const char *name, IO *ein, IO *aus)
{
	StrBuf *buf = sb_acquire();
	std_head(name, buf, ein, aus);
	std_copy(name, buf, ein, aus, NULL);
	sb_release(buf);
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
		io_xprintf(aus, "\\mpage[%s] %s\n",
			Secnum ? Secnum : suffix, fname);
		memfree(fname);
	}

	io_copy(ein, aus);
}
