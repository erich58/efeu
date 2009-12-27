/*
:*: table with format strings
:de: Tabelle mit Formatdefinitionen

$Copyright (C) 2002 Erich Frühstück
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

#include <EFEU/Resource.h>
#include <EFEU/Debug.h>
#include <EFEU/procenv.h>
#include <EFEU/vecbuf.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <EFEU/parsub.h>
#include <ctype.h>

#define	MSG_PFX	"messages"
#define	MSG_SFX	"msg"

#define	ERRMSG	"%s: message catalog %#s not found.\n"
#define	M_LOAD	"%s: loading message catalog %#s.\n"
#define	M_NOKEY	"%s: key %#s not found in message catalog %#s.\n"

#define	LOG(level)	LogOut("msg", level)

/*	Formateintrag
*/

typedef struct {
	char *key;	/* Kennung */
	char *fmt;	/* Formatdefinition */
} SubEntry;

static int SubEntry_comp (const void *ap, const void *bp)
{
	const SubEntry *a = ap;
	const SubEntry *b = bp;

	return mstrcmp(a->key, b->key);
}


/*	Haupteinträge
*/

typedef struct {
	char *name;	/* Formatname */
	SubEntry *tab;	/* Formattabelle */
	size_t dim;	/* Zahl der Einträge */
} MainEntry;

static int MainEntry_comp (const void *ap, const void *bp)
{
	const MainEntry *a = ap;
	const MainEntry *b = bp;

	return mstrcmp(a->name, b->name);
}

static IO *ftab_open (const char *name)
{
	char *p;
	IO *io;

	p = fsearch(ApplPath, MSG_PFX, name, MSG_SFX);

	if	(p == NULL)
	{
		io_xprintf(LOG(DBG_NOTE), ERRMSG, ProgName, name);
		return NULL;
	}

	io_xprintf(LOG(DBG_DEBUG), M_LOAD, ProgName, p);
	io = io_fileopen(p, "r");
	memfree(p);
	return io;
}

static void load_fmt (IO *io, StrBuf *buf)
{
	char *delim = "\n";
	int c;

	do	c = io_getc(io);
	while	(c == ' ' || c == '\t');

	if	(c == '"')
	{
		delim = "\"";
	}
	else	io_ungetc(c, io);

	while ((c = io_xgetc(io, delim)) != EOF)
		if (c) sb_putc(c, buf);

	io_getc(io);
	sb_putc(0, buf);
}


static void ftab_show (IO *io, MainEntry *entry)
{
	int n;

	if	(!io)	return;

	for (n = 0; n < entry->dim; n++)
		io_xprintf(io, "%s: %#s\n",
			entry->tab[n].key, entry->tab[n].fmt);
}

static void ftab_load (IO *io, MainEntry *entry, int endmark)
{
	StrBuf *buf;
	size_t pos, n;
	char *p;
	int c;

	buf = sb_acquire();
	entry->dim = 0;

	while ((c = io_skipcom(io, NULL, 0)) != EOF)
	{
		if	(c == endmark)	break;
		if	(isspace(c))	continue;

		do
		{
			sb_putc(c, buf);
			c = io_getc(io);
		}
		while	(c != EOF && c != ',' && !isspace(c));

		sb_putc(0, buf);
		load_fmt(io, buf);
		entry->dim++;
	}

	if	(!entry->dim)	return;

	pos = sb_getpos(buf);
	entry->tab = lmalloc(entry->dim * sizeof(SubEntry) + pos);
	p = (char *) (entry->tab + entry->dim);
	memcpy(p, buf->data, pos);
	sb_release(buf);

	for (n = 0; n < entry->dim; n++)
	{
		entry->tab[n].key = p;

		while (*p != 0) p++;
		p++;

		entry->tab[n].fmt = p;

		while (*p != 0) p++;
		p++;
	}

	qsort(entry->tab, entry->dim, sizeof(entry->tab[0]), SubEntry_comp);
	ftab_show(LOG(DBG_DEBUG), entry);
}

static VECBUF(FormatTab, 32, sizeof(MainEntry));

void FormatTabLoad (IO *io, const char *name, int endkey)
{
	MainEntry entry, *ptr;

	entry.name = (char *) name;
	entry.tab = NULL;
	entry.dim = 0;

	ptr = vb_search(&FormatTab, &entry, MainEntry_comp, VB_ENTER);

	if	(ptr->tab)
		lfree(ptr->tab);

	ptr->tab = NULL;
	ptr->dim = 0;
	ftab_load(io, ptr, endkey);
}

char *FormatTabEntry (const char *name, const char *key)
{
	MainEntry entry, *ptr;
	SubEntry sub, *s;

	sub.key = (char *) key;
	sub.fmt = NULL;

	entry.name = (char *) name;
	entry.tab = &sub;
	entry.dim = 0;

	ptr = vb_search(&FormatTab, &entry, MainEntry_comp, VB_ENTER);

	if	(ptr->tab == &sub)
	{
		IO *io;

		ptr->name = mstrcpy(ptr->name);
		ptr->tab = NULL;
		io = ftab_open(ptr->name ? ptr->name : ProgName);

		if	(io)
		{
			ftab_load(io, ptr, EOF);
			io_close(io);
		}
	}

	s = ptr->dim ? bsearch(&sub, ptr->tab, ptr->dim,
		sizeof(ptr->tab[0]), SubEntry_comp) : NULL;

	if	(s == NULL)
	{
		io_xprintf(LOG(DBG_NOTE), M_NOKEY,
			ProgName, key, name);
		return NULL;
	}

	io_xprintf(LOG(DBG_TRACE), "GetFormat(%#s,%#s): %#s\n",
		ptr->name, s->key, s->fmt);
	return s->fmt;
}

void FormatTabShow (IO *io, const char *name, const char *fmt)
{
	MainEntry entry;
	char *tab[3];
	IO *in;
	int i;

	entry.name = (char *) name;
	entry.tab = NULL;
	entry.dim = 0;

	in = ftab_open(name ? name : ProgName);

	if	(in)
	{
		ftab_load(in, &entry, EOF);
		io_close(in);
	}

	tab[0] = entry.name;

	for (i = 0; i < entry.dim; i++)
	{
		tab[1] = entry.tab[i].key;
		tab[2] = entry.tab[i].fmt;
		io_psubvec(io, fmt, 3, tab);
	}

	lfree(entry.tab);
}
