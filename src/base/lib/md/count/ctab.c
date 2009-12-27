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


typedef struct {
	void (*init) (void *data, size_t *idx, size_t dim);
	size_t dim;
	size_t *idx;
} INIT;

static void subinit (INIT *init, int depth, mdaxis *axis, char *data);
static void dummy (void *data);


static void dummy(void *data)
{
	;
}


/*	Zähltabelle aus IO-Struktur generieren
*/

mdmat *md_ioctab(const char *title, IO *io,
	MdClassTab *gtab, MdCntObj *counter)
{
	mdmat *tab;
	mdaxis **ptr;

/*	Matrixheader generieren
*/
	tab = new_mdmat();
	tab->title = mstrcpy(title);
	tab->axis = NULL;
	ptr = &tab->axis;

/*	Matrixdimensionen initialisieren
*/
	while ((*ptr = md_ctabaxis(io, gtab)) != NULL)
		ptr = &(*ptr)->next;

	md_ctabinit(tab, counter);
	return tab;
}


/*	Zähltabelle initialisieren
*/

void md_ctabinit (mdmat *tab, MdCntObj *counter)
{
	if	(counter == NULL)
		counter = stdcount;

	if	(counter->add == NULL)
	{
		dbg_error(NULL, "[mdmat:41]", NULL);
		counter->add = dummy;
	}

/*	Datenmatrix initialisieren
*/
	tab->type = mdtype(counter->type);
	tab->priv = counter;
	tab->size = md_size(tab->axis, tab->type->size);
	tab->data = memalloc(tab->size);

	if	(counter->init)
	{
		INIT init;

		init.init = counter->init;
		init.dim = md_dim(tab->axis);
		init.idx = memalloc(init.dim * sizeof(size_t));
		memset(init.idx, 0, init.dim * sizeof(size_t));

		subinit(&init, 0, tab->axis, (char *) tab->data);
	}
}


/*	Zähltabelle aus Definitionsstring generieren
*/

mdmat *md_ctab(const char *title, const char *def,
	MdClassTab *gtab, MdCntObj *counter)
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
	else	init->init(ptr, init->idx, init->dim);
}
