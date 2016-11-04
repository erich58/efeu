/*
:de:	Datenmatrix aus Tupeln generieren
:*:	create mdmat object from tuples

$Copyright (C) 2001 Erich Frühstück
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

#include <DB/PG.h>
#include <EFEU/mdmat.h>
#include <EFEU/mstring.h>
#include <EFEU/strbuf.h>
#include <EFEU/Debug.h>
#include <EFEU/parsearg.h>

#if	HAS_PQ

#define	M_NODATA	"no data available.\n"
#define	M_UNDEF	"undefined field \"$1\".\n"
#define	M_NF	"$0: field number $1 is out of range 0..$2.\n"
#define	M_NT	"$0: tuple number $1 is out of range 0..$2.\n"

int PG_expandlim = 32;

typedef struct {
	char *data;
	int idx;
} IDX;

static int cmp_name (const void *p1, const void *p2)
{
	register const IDX *i1 = p1;
	register const IDX *i2 = p2;
	return strcmp(i1->data, i2->data);
}

static int cmp_idx (const void *p1, const void *p2)
{
	register const IDX *i1 = p1;
	register const IDX *i2 = p2;

	if	(i1->idx < i2->idx)	return -1;
	else if	(i1->idx > i2->idx)	return 1;
	else				return 0;
}

static int getidx (PG *pg, const char *def)
{
	int n;

	if	(*def == '#')
	{
		n = atoi(def + 1);

		if	(n < 0)	n += PQnfields(pg->res);

		if	(n >= PQnfields(pg->res))
			n = -1;
	}
	else	n = PQfnumber(pg->res, def);

	if	(n < 0)
		PG_info(pg, M_UNDEF, "s", def);

	return n;
}

static char *inc_lbl (const char *lbl, const char *def)
{
	StrBuf *buf;
	IO *io;
	char *p;

	buf = sb_acquire();	

	for (; *def != 0; def++)
	{
		if	(*def == '#')
		{
			if	(def[1] == '#')
			{
				def++;
				sb_putc(*def, buf);
			}
			else	sb_printf(buf, "%#s", lbl);
		}
		else	sb_putc(*def, buf);
	}

	sb_begin(buf);
	io = io_strbuf(buf);
	p = Obj2str(Parse_term(io, 0));
	io_close(io);
	sb_release(buf);
	return p;
}

static mdaxis *make_axis (StrPool *sbuf, PG *pg, const char *def, int n)
{
	AssignArg *args;
	VecBuf xbuf;
	mdaxis *axis;
	char **xp;
	int field;
	IDX *idx;
	int i, j;
	size_t k;
	
	args = assignarg(def, NULL, ",");
	field = getidx(pg, args->arg ? args->arg : args->name);

	if	(field < 0)
	{
		memfree(args);
		return NULL;
	}

	idx = memalloc(n * sizeof(IDX));

	for (i = 0; i < n; i++)
	{
		idx[i].data = PQgetvalue(pg->res, i, field);
		idx[i].idx = i;
	}
		
	qsort(idx, n, sizeof(IDX), cmp_name);
	vb_init(&xbuf, 32, sizeof(char **));

	k = xbuf.used;
	xp = vb_next(&xbuf);
	*xp = mstrcpy(idx[0].data);
	idx[0].data = (void *) k;

	for (i = 1; i < n; i++)
	{
		if	(strcmp(*xp, idx[i].data) != 0)
		{
			char *p;
			int rval;
			int flag;
			
			p = *xp;
			flag = 1;

			for (j = 0; args->opt; j++)
			{
				if	(PG_expandlim && j >= PG_expandlim)
					break;

				p = inc_lbl(p, args->opt);

				if	(p == NULL)	break;

				rval = strcmp(p, idx[i].data);

				if	(rval > 0)
				{
					memfree(p);
					break;
				}
				else if	(rval == 0)
				{
					k = xbuf.used;
					xp = vb_next(&xbuf);
					*xp = p;
					flag = 0;
					break;
				}

				xp = vb_next(&xbuf);
				*xp = p;
			}

			if	(flag)
			{
				k = xbuf.used;
				xp = vb_next(&xbuf);
				*xp = mstrcpy(idx[i].data);
			}
		}

		idx[i].data = (void *) k;
	}

	qsort(idx, n, sizeof(IDX), cmp_idx);

	axis = new_axis(sbuf, xbuf.used);
	axis->i_name = StrPool_xadd(sbuf, args->name);

	for (i = 0, xp = xbuf.data; i < xbuf.used; i++)
		axis->idx[i].i_name = StrPool_add(sbuf, xp[i]);

	axis->priv = idx;
	vb_free(&xbuf);
	memfree(args);
	return axis;
}

/*
:de:
Die Funktion |$1| generiert eine Datenmatrix aus der letzten
Datenbankabfrage.
:*:
The function |$1| creates a mdmat object from last query.
*/

mdmat *PG_mdmat (PG *pg, const EfiType *type,
	const char *valdef, const char *axis)
{
	size_t i, n;
	char **list;
	mdmat *md;
	mdaxis *p, **ptr;
	char *data;
	char *value;
	int ntuples;
	int field;
	EfiConv konv;
	EfiFunc *f_add;
	void *buf;
	char *title;

	if	(pg == NULL || pg->res == NULL || type == NULL)
		return NULL;

	ntuples = PQntuples(pg->res);

	if	(ntuples == 0)
	{
		PG_info(pg, M_NODATA, NULL);
		return NULL;
	}

	if	(!Md_ConvDef(&konv, &Type_str, type))
		return NULL;

	if	((f_add = mdfunc_add(type)) == NULL)
		return NULL;

	field = getidx(pg, valdef);

	if	(field < 0)	return NULL;

/*	create mdmat-object
*/
	md = new_mdmat();
	md->sbuf = NewStrPool();
	md->type = (EfiType *) type;
	title = msprintf("%s by %s", valdef, axis);
	md->i_name = StrPool_add(md->sbuf, title);
	memfree(title);
	ptr = &md->axis;

	n = mstrsplit(axis, ",", &list);

	for (i = 0; i < n; i++)
	{
		*ptr = make_axis(md->sbuf, pg, list[i], ntuples);

		if	(*ptr != NULL)
			ptr = &(*ptr)->next;
	}
	
	memfree(list);

/*	set data
*/
	md_alloc(md);
	buf = memalloc(type->size);

	for (i = 0; i < ntuples; i++)
	{
		data = md->data;

		for (p = md->axis; p != NULL; p = p->next)
		{
			IDX *idx = p->priv;
			data += (size_t) idx[i].data * p->size;
		}

		value = PQgetvalue(pg->res, i, field);
		ConvData(&konv, buf, &value);
		CallVoidFunc(f_add, data, buf);
	}

/*	clean private data
*/
	CleanData(type, buf, 1);
	memfree(buf);

	for (p = md->axis; p != NULL; p = p->next)
	{
		memfree(p->priv);
		p->priv = NULL;
	}

	return md;
}

#endif

/*
$SeeAlso
\mref{PG_connect(3)},
\mref{PG_query(3)},
\mref{SetupPG(3)},
\mref{PG(7)}, \mref{mdmat(7)}.\br
PostgreSQL Programmer's Guide.
*/
