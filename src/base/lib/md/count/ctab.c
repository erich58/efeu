/*
Zählerstruktur generieren

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

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <ctype.h>
#include <EFEU/preproc.h>


typedef struct {
	MdCount *cnt;
	void (*init) (MdCount *cnt, void *data, size_t *idx, size_t dim);
	size_t dim;
	size_t *idx;
} INIT;

static void subinit (INIT *init, int depth, mdaxis *axis, char *data);

static void dummy (MdCount *cnt, void *data, void *buf)
{
	;
}


/*	Zähltabelle aus IO-Struktur generieren
*/

mdmat *md_ioctab (const char *title, IO *io,
	MdCountPar *gtab, MdCount *counter)
{
	mdmat *tab;
	mdaxis **ptr;

/*	Matrixheader generieren
*/
	tab = new_mdmat();
	tab->sbuf = rd_refer(gtab->pool);
	tab->i_name = StrPool_xadd(tab->sbuf, title);
	tab->axis = NULL;
	ptr = &tab->axis;

/*	Matrixdimensionen initialisieren
*/
	while ((*ptr = md_ctabaxis(io, gtab)) != NULL)
		ptr = &(*ptr)->next;

	md_ctabinit(tab, MdCountList_create(counter));
	return tab;
}

static void put_name (const char *name, IO *io)
{
	if	(isdigit((unsigned char) *name))
		io_putc('_', io);

	for (; *name; ++name)
		io_putc((*name == '_' || isalnum((unsigned char) *name)) ?
			*name : '_', io);
}

static void put_type (const char *str, IO *io)
{
	if	(str[0] == '[')
	{
		char *head;
		char *name;
		IO *cin;

		head = mstrcut(str + 1, &name, "]", 0);
		cin = io_cmdpreproc(io_mstr(head));
		CmdEval(cin, NULL);
		io_close(cin);
		str = name;
	}

	io_puts(str, io);
}

/*	Zähltabelle initialisieren
*/

int md_ctabinit (mdmat *tab, MdCountList *list)
{
	MdCountList *cl;
	int need_init;

	if	(list == NULL)
		list = MdCountList_create(stdcount);

	if	(list->cnt->add == NULL)
	{
		dbg_error(NULL, "[mdmat:41]", NULL);
		list->cnt->add = dummy;
	}

	need_init = 0;

/*	Datenmatrix initialisieren
*/
	if	(list->next)
	{
		EfiStruct *vlist;
		IO *io;

		io = io_tmpbuf(0);

		for (cl = list; cl != NULL; cl = cl->next)
		{
			if	(cl->cnt->init)
				need_init = 1;

			put_type(cl->cnt->type, io);
			io_puts(" ", io);
			put_name(cl->cnt->name, io);
			io_puts(";", io);
		}

		io_rewind(io);
		io = io_cmdpreproc(io);
		vlist = GetStruct(NULL, io, EOF);
		io_close(io);
		tab->type = MakeStruct(NULL, NULL, vlist);

		vlist = tab->type->list;

		for (cl = list; cl != NULL; cl = cl->next)
		{
			cl->offset = vlist->offset;
			vlist = vlist->next;
		}
	}
	else
	{
		if	(list->cnt->init)
			need_init = 1;

		tab->type = mdtype(list->cnt->type);
	}

	tab->x_priv = list;
	md_alloc(tab);

	if	(need_init)
	{
		INIT init;

		init.dim = md_dim(tab->axis);
		init.idx = memalloc(init.dim * sizeof(size_t));
		memset(init.idx, 0, init.dim * sizeof(size_t));

		for (cl = list; cl != NULL; cl = cl->next)
		{
			if	(cl->cnt->init)
			{
				init.cnt = cl->cnt;
				init.init = cl->cnt->init;
				subinit(&init, 0, tab->axis,
					(char *) tab->data + cl->offset);
			}
		}

		memfree(init.idx);
		return 1;
	}

	return 0;
}


/*	Zähltabelle aus Definitionsstring generieren
*/

mdmat *md_ctab (const char *title, const char *def,
	MdCountPar *gtab, MdCount *counter)
{
	IO *io;
	mdmat *md;

	io = io_cstr(def);
	md = md_ioctab(title, io, gtab, counter);
	io_close(io);
	return md;
}


/*	Initialisierungshilfsprogramm
*/

static void subinit (INIT *init, int depth, mdaxis *axis, char *ptr)
{
	if	(axis != NULL)
	{
		int i;

		for (i = 0; i < axis->dim; i++)
		{
			init->idx[depth] = i;
			subinit(init, depth + 1, axis->next, ptr);
			ptr += axis->size;
		}
	}
	else	init->init(init->cnt, ptr, init->idx, init->dim);
}
