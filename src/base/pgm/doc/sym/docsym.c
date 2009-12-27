/*
Ausgabekonvertierung laden

$Copyright (C) 1999 Erich Frühstück
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

#include <DocSym.h>
#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/preproc.h>
#include <ctype.h>

#define	FILE_EXT	"sym"

static int sym_cmp (DocSymEntry_t *a, DocSymEntry_t *b)
{
	return mstrcmp(a->key, b->key);
}

static void cpy_str (io_t *io, strbuf_t *buf)
{
	int c;

	io_protect(io, 1);

	while ((c = io_xgetc(io, "%s")) != EOF)
		sb_putc(c, buf);

	io_protect(io, 0);
	sb_putc(0, buf);
}

static void load (DocSym_t *sym, io_t *io)
{
	int c;

	io = io_lnum(io);

	while ((c = io_eat(io, "%s")) != EOF)
	{
		if	(isalpha(c))
		{
			cpy_str(io, &sym->buf);
			io_eat(io, " \t");
			cpy_str(io, &sym->buf);
			sym->dim++;
		}

		io_skip(io, "\n");
	}

	io_close(io);
}

static char *next_str (char **ptr)
{
	char *p = *ptr;

	while (**ptr != 0)
		(*ptr)++;

	(*ptr)++;
	return p;
}

static void reorg (DocSym_t *sym)
{
	DocSymEntry_t *entry;
	char *ptr;
	int i;

	sym->tab = memalloc(sym->dim * sizeof(DocSymEntry_t));
	ptr = (char *) sym->buf.data;

	for (i = sym->dim, entry = sym->tab; i-- > 0; entry++)
	{
		entry->key = next_str(&ptr);
		entry->fmt = next_str(&ptr);
	}

	if	(sym->dim > 1)
		qsort(sym->tab, sym->dim, sizeof(DocSymEntry_t),
			(comp_t) sym_cmp);
}

DocSym_t *DocSym (const char *name)
{
	DocSym_t *sym;

	sym = memalloc(sizeof(DocSym_t));
	load(sym, io_findopen(CFGPATH, name, FILE_EXT, "rd"));
	reorg(sym);
	return sym;
}

char *DocSym_get (DocSym_t *sym, const char *name)
{
	DocSymEntry_t key, *ptr;

	if	(sym == NULL || sym->dim == 0)
		return NULL;

	key.key = (char *) name;
	key.fmt = NULL;
	ptr = bsearch(&key, sym->tab, sym->dim, sizeof(DocSymEntry_t),
		(comp_t) sym_cmp);
	return ptr ? ptr->fmt : NULL;
}

int DocSym_print (io_t *io, DocSym_t *sym)
{
	DocSymEntry_t *ptr;
	int i, n;

	if	(sym == NULL)
		return 0;

	for (n = 0, i = sym->dim, ptr = sym->tab; i-- > 0; ptr++)
		n += io_printf(io, "%s\t%#s\n", ptr->key, ptr->fmt);

	return n;
}

void DocSym_free (DocSym_t *sym)
{
	if	(sym)
	{
		sb_clear(&sym->buf);
		memfree(sym->tab);
		memfree(sym);
	}
}
