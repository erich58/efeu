/*
Dokumentbuffer

$Copyright (C) 2001 Erich Frühstück
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

#include "DocBuf.h"


static char *syn_head = "\\Synopsis\n";

static char *SecHead (int n)
{
	switch (n)
	{
	case BUF_SYN:	return syn_head;
	case BUF_DESC:	return "\\Description\n";
	case BUF_EX:	return "\\Examples\n";
	case BUF_SEE:	return "\\SeeAlso\n";
	case BUF_DIAG:	return "\\Diagnostics\n";
	case BUF_NOTE:	return "\\Notes\n";
	case BUF_WARN:	return "\\Warnings\n";
	case BUF_ERR:	return "\\Errors\n";
/*
	case BUF_CPR:	return "\\Copyright\nCopyright\n";
*/
	default:	return NULL;
	}
}


void copy_protect (const char *str, IO *io)
{
	if	(!str || !io)	return;

	for (; *str; str++)
	{
		switch (*str)
		{
		case '<':
		case '>':
		case '\\':
		case '|':
		case '[':
		case '*':
		case '#':
		case '$':
		case '&':
			io_putc('\\', io);
			break;
		case '/':
			if	(str[1] == '/' || str[1] == '*')
				io_putc('\\', io);
			break;
		default:
			break;
		}

		io_putc(*str, io);
	}
}

void DocBuf_init (DocBuf *doc)
{
	int i;

	memset(doc, 0, sizeof *doc);
	doc->cmdpar = NULL;
	doc->synopsis = sb_create(0);
	doc->source = sb_create(0);

	for (i = 0; i < BUF_DIM; i++)
		doc->tab[i] = sb_create(0);
}

void DocBuf_write (DocBuf *doc, IO *io)
{
	int i;

	io_puts("\\Name\n", io);
	io_printf(io, "%s -- ", doc->var[VAR_NAME]);
	io_printf(io, "%s\n", doc->var[VAR_TITLE]);

	if	(doc->var[VAR_HEAD] || sb_getpos(doc->synopsis))
	{
		io_puts(syn_head, io);
		syn_head = "\n";
		io_puts("\\code\n", io);

		if	(doc->var[VAR_HEAD])
		{
			io_puts("\\#include ", io);
			copy_protect(doc->var[VAR_HEAD], io);
			io_puts("\n\n", io);
		}

		if	(sb_getpos(doc->synopsis))
		{
			sb_putc(0, doc->synopsis);
			io_puts((char *) doc->synopsis->data, io);
		}

		io_puts("\\end\n", io);
	}

	for (i = 0; i < BUF_DIM; i++)
	{
		if	(sb_getpos(doc->tab[i]))
		{
			io_puts(SecHead(i), io);
			sb_putc(0, doc->tab[i]);
			io_puts((char *) doc->tab[i]->data, io);
		}

		sb_destroy(doc->tab[i]);
	}

	sb_destroy(doc->synopsis);
	sb_destroy(doc->source);

	if	(doc->var[VAR_COPYRIGHT])
		io_printf(io, "\\Copyright\nCopyright %s\n",
			doc->var[VAR_COPYRIGHT]);

	for (i = 0; i < VAR_DIM; i++)
		memfree(doc->var[i]);
}

