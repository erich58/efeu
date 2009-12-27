/*
Abhängigkeitslisten

$Copyright (C) 1999 Erich Frühstück
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

#include <EFEU/ftools.h>
#include <EFEU/MakeDepend.h>

#define	BREAK_COL	70	/* Spaltenpostion für Fortsetzungszeile */

int MakeDepend = 0;

VECBUF(TargetList, 16, sizeof(char *));
VECBUF(DependList, 16, sizeof(char *));

static int cmp_name (const char **a, const char **b)
{
	return mstrcmp(*a, *b);
}

static void add_name (vecbuf_t *tab, const char *name)
{
	char *p, **ptr;

	if	(name == NULL)	return;
	if	(*name == '|')	return;

	p = mstrcpy(name);
	ptr = vb_search(tab, &p, (comp_t) cmp_name, VB_REPLACE); 

	if	(ptr)	memfree(*ptr);
}

void AddTarget (const char *name)
{
	add_name(&TargetList, name);
}

void AddDepend (const char *name)
{
	add_name(&DependList, name);
}

static int put_name (io_t *io, int col, const char *name)
{
	if	(col)
	{
		io_putc(' ', io);
		col++;

		if	(name && col + strlen(name) >= BREAK_COL)
		{
			io_puts("\\\n ", io);
			col = 1;
		}
	}
			
	return col + io_puts(name, io);
}

void MakeTaskRule (io_t *io, const char *name)
{
	int n, p;
	char **ptr;

	io_putc('\n', io);
	p = put_name(io, 0, name);
	p += io_puts("::", io);

	for (n = TargetList.used, ptr = TargetList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	io_putc('\n', io);
}

void MakeCleanRule (io_t *io, const char *name)
{
	int n, p;
	char **ptr;

	io_puts(name, io);
	io_puts(" ::\n\t", io);
	p = 8;
	p = put_name(io, 0, "rm -f");

	for (n = TargetList.used, ptr = TargetList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	io_putc('\n', io);
}

void MakeTargetRule (io_t *io, const char *cmd)
{
	int n, p;
	char **ptr;

	io_putc('\n', io);
	p = 0;

	for (n = TargetList.used, ptr = TargetList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	if	(TargetList.used)
	{
		io_putc(':', io);
		p++;
	}

	for (n = DependList.used, ptr = DependList.data; n-- > 0; ptr++)
		p = put_name(io, p, *ptr);

	io_putc('\n', io);

	if	(cmd)
	{
		io = io_lmark(io_refer(io), "\t", NULL, 0);
		io_puts(cmd, io);
		io_putc('\n', io);
		io_close(io);
	}
}

void MakeDependRule (io_t *io)
{
	MakeTargetRule(io, NULL);
}
