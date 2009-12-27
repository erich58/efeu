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


static Label glabel = { "GES", "Insgesamt" };

static MdClass gsel = {
	"GES",		/* name */
	"Insgesamt",	/* desc */
	1,		/* dim */
	&glabel,	/* label */
	NULL,		/* classify */
};


static ALLOCTAB(sdef_tab, 16, sizeof(MdCntGrp));


/*	Eintrag initialisieren
*/

mdaxis *md_classaxis (const char *name, ...)
{
	va_list list;
	MdClass *class;
	mdaxis *grp;
	MdCntGrp *sdef, **x;
	size_t dim;
	IO *tmp;
	int i, j;

	if	(name == NULL)	return NULL;

	sdef = NULL;
	x = &sdef;
	dim = 0;
	tmp = io_tmpbuf(0);
	io_putstr(name, tmp);

	va_start(list, name);

	while ((class = va_arg(list, MdClass *)) != NULL)
	{
		for (j = 0; j < class->dim; j++)
			io_putstr(class->label[j].name, tmp);

		*x = new_data(&sdef_tab);
		(*x)->next = NULL;
		(*x)->cdef = class;
		(*x)->flag = 0;
		(*x)->idx = 0;
		x = &(*x)->next;
		dim += class->dim;
	}

	va_end(list);

	if	(dim == 0)
	{
		sdef = new_data(&sdef_tab);
		sdef->next = NULL;
		sdef->cdef = &gsel;
		sdef->flag = 0;
		sdef->idx = 0;
		dim = gsel.dim;
		io_putstr(NULL, tmp);
	}

	io_rewind(tmp);
	grp = new_axis(dim);
	grp->name = io_getstr(tmp);
	grp->priv = sdef;

	for (i = 0; i < dim; i++)
		grp->idx[i].name = io_getstr(tmp);

	io_close(tmp);
	return grp;
}

mdaxis *md_ctabaxis (IO *io, MdClassTab *gtab)
{
	mdaxis *grp;
	MdCntGrp *sdef, **x;
	size_t dim;
	mdlist *def;
	IO *tmp = NULL;
	int i, j;

/*	Achsendefinition bestimmen
*/
	if	((def = io_mdlist(io, MDLIST_LISTOPT)) == NULL)
		return NULL;

/*	Zählerliste aufbauen
*/
	sdef = NULL;
	x = &sdef;
	dim = 0;
	tmp = io_tmpbuf(0);
	io_putstr(def->name, tmp);

	for (i = 0; i < def->dim; i++)
	{
		MdClass *s = MdClass_get(gtab, def->list[i]);

		if	(s == NULL || s->dim == 0)
		{
			dbg_note(NULL, "[mdmat:44]", "m", def->list[i]);
			continue;
		}

		s = md_subclass(s, def->lopt[i]);

		for (j = 0; j < s->dim; j++)
			io_putstr(s->label[j].name, tmp);

		*x = new_data(&sdef_tab);
		(*x)->next = NULL;
		(*x)->cdef = s;
		(*x)->flag = 0;
		(*x)->idx = 0;
		x = &(*x)->next;
		dim += s->dim;
	}

/*	Keine gültige Selektionsdefinition: Gesamtzähler verwenden
*/
	if	(dim == 0)
	{
		sdef = new_data(&sdef_tab);
		sdef->next = NULL;
		sdef->cdef = &gsel;
		sdef->flag = 0;
		sdef->idx = 0;
		dim = gsel.dim;
		io_putstr(NULL, tmp);
	}

/*	Indexliste generieren
*/
	io_rewind(tmp);
	grp = new_axis(dim);
	grp->name = io_getstr(tmp);
	grp->priv = sdef;

	for (i = 0; i < dim; i++)
		grp->idx[i].name = io_getstr(tmp);

	io_close(tmp);
	return grp;
}
