/*
Sortierung/Abfragen von Formatdefinitionen

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

#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/io.h>
#include <EFEU/parsub.h>
#include <EFEU/patcmp.h>
#include <EFEU/msgtab.h>
#include <EFEU/procenv.h>
#include <EFEU/Debug.h>

#define	MSG_PFX	"messages"
#define	MSG_SFX	"msg"

typedef struct {
	char *name;
	vecbuf_t buf;
} MSGTAB_t;

#define	MSG_BLKSIZE	32

#define	ERRMSG	"$!: Formatdatei %s nicht gefunden.\n"

static int cmp_msg(MSGTAB_t *a, MSGTAB_t *b)
{
	return mstrcmp(a->name, b->name);
}

static int cmp_entry(msgdef_t *a, msgdef_t *b)
{
	return (a->num - b->num);
}

static VECBUF(msgtab, 16, sizeof(MSGTAB_t));


static int do_loadmsg(io_t *io, vecbuf_t *vb, const char *delim);

static MSGTAB_t *new_msgtab(const char *name)
{
	MSGTAB_t key, *tab;

	key.name = mstrcpy(name);
	vb_init(&key.buf, MSG_BLKSIZE, sizeof(msgdef_t));
	tab = vb_search(&msgtab, &key, (comp_t) cmp_msg, VB_ENTER);
	memfree(key.name);
	return tab;
}


/*	Suche nach einer Formatdefinition
*/

char *getmsg (const char *name, int num, const char *defval)
{
	MSGTAB_t key, *tab;
	msgdef_t mkey, *msg;

	key.name = (char *) name;
	tab = vb_bsearch(&msgtab, &key, (comp_t) cmp_msg);

	if	(tab == NULL)
	{
		char *p;
		io_t *io;

		tab = new_msgtab(name);

		if	(name == NULL)	name = ProgName;

		p = fsearch(ApplPath, MSG_PFX, name, MSG_SFX);
		io = p ? io_fileopen(p, "r") : NULL;
		memfree(p);

		if	(io == NULL)
		{
			io_printf(LogOut("msgtab", DBG_ERR), ERRMSG, name);
			return (char *) defval;
		}

		do_loadmsg(io, &tab->buf, NULL);
		io_close(io);
	}

	mkey.num = num;
	msg = tab ? vb_bsearch(&tab->buf, &mkey, (comp_t) cmp_entry) : NULL;
	return (msg ? msg->fmt : (char *) defval);
}

static int do_loadmsg(io_t *io, vecbuf_t *vb, const char *delim)
{
	msgdef_t *def;
	char *s;
	int c;

	while ((c = io_eat(io, "%s")) != EOF)
	{
		if	(listcmp(delim, c))
		{
			io_getc(io);
			break;
		}

		def = vb_next(vb);
		s = io_mgets(io, ",%s");
		def->num = atoi(s);
		memfree(s);

		c = io_eat(io, ", \t");

		if	(c == '"')
		{
			io_getc(io);
			def->fmt = io_xgets(io, "\"");
			io_getc(io);
			io_eat(io, ", \t");
		}
		else
		{
			def->fmt = io_xgets(io, "\n");

			if	(mstrcmp(def->fmt, "NULL") == 0)
			{
				memfree(def->fmt);
				def->fmt = NULL;
			}
		}
	}

	vb_qsort(vb, (comp_t) cmp_entry);
	return c;
}


int io_loadmsg(io_t *io, const char *name, const char *delim)
{
	MSGTAB_t *tab;

	tab = new_msgtab(name);
	do_loadmsg(io, &tab->buf, delim);
	return tab->buf.used;
}
