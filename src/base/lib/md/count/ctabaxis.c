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
	NULL,		/* name */
	NULL,		/* desc */
	1,		/* dim */
	&glabel,	/* label */
	NULL,		/* classify */
};


static ALLOCTAB(sdef_tab, "MdCntGrp", 16, sizeof(MdCntGrp));


/*	Eintrag initialisieren
*/

static STRBUF(buf_label, 0);

mdaxis *md_classaxis (const char *name, ...)
{
	va_list list;
	MdClass *class;
	mdaxis *grp;
	MdCntGrp *sdef, **x;
	size_t dim;
	StrPool *sbuf;
	size_t offset;
	int i, j;

	if	(name == NULL)	return NULL;

	sdef = NULL;
	x = &sdef;
	dim = 0;
	sbuf = NewStrPool();
	offset = StrPool_add(sbuf, name);
	buf_label.pos = 0;

	va_start(list, name);

	while ((class = va_arg(list, MdClass *)) != NULL)
	{
		if	(class->desc)
		{
			if	(buf_label.pos)
				sb_puts("; ", &buf_label);

			sb_puts(class->desc, &buf_label);
		}

		for (j = 0; j < class->dim; j++)
		{
			StrPool_xadd(sbuf, class->label[j].name);
			StrPool_xadd(sbuf, class->label[j].desc);
		}

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
		StrPool_xadd(sbuf, NULL);
		StrPool_xadd(sbuf, NULL);
	}

	grp = new_axis(sbuf, dim);
	grp->i_name = offset;

	if	(buf_label.pos)
	{
		sb_putc(0, &buf_label);
		grp->i_desc = StrPool_add(sbuf, (char *) buf_label.data);
	}

	grp->priv = sdef;

	for (i = 0; i < dim; i++)
	{
		offset += strlen(StrPool_get(sbuf, offset)) + 1;
		grp->idx[i].i_name = offset;
		offset += strlen(StrPool_get(sbuf, offset)) + 1;
		grp->idx[i].i_desc = offset;
	}

	return grp;
}

mdaxis *md_ctabaxis (IO *io, MdCountPar *gtab)
{
	mdaxis *grp;
	MdCntGrp *sdef, **x;
	size_t dim;
	size_t offset;
	mdlist *def;
	int i, j;

/*	Achsendefinition bestimmen
*/
	if	((def = io_mdlist(io, MDLIST_LISTOPT)) == NULL)
		return NULL;

/*	Klassifikationsliste aufbauen
*/
	sdef = NULL;
	x = &sdef;
	dim = 0;
	offset = StrPool_xadd(gtab->pool, def->name);
	buf_label.pos = 0;

	for (i = 0; i < def->dim; i++)
	{
		MdClass *s;
		
		if	(def->list[i] == NULL)	continue;

		if	((s = MdClass_get(gtab, def->list[i])) == NULL)
		{
			log_error(md_err, "[mdmat:44]", "s", def->list[i]);
			continue;
		}

		s = md_subclass(s, def->lopt[i]);

		if	(s->desc)
		{
			if	(buf_label.pos)
				sb_puts("; ", &buf_label);

			sb_puts(s->desc, &buf_label);
		}

		for (j = 0; j < s->dim; j++)
		{
			StrPool_xadd(gtab->pool, s->label[j].name);
			StrPool_xadd(gtab->pool, s->label[j].desc);
		}

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
		*x = new_data(&sdef_tab);
		(*x)->next = NULL;
		(*x)->cdef = &gsel;
		(*x)->flag = 0;
		(*x)->idx = 0;
		dim = gsel.dim;
		StrPool_xadd(gtab->pool, NULL);
		StrPool_xadd(gtab->pool, NULL);
	}

/*	Indexliste generieren
*/
	grp = new_axis(gtab->pool, dim);
	grp->i_name = offset;

	if	(buf_label.pos)
	{
		sb_putc(0, &buf_label);
		grp->i_desc = StrPool_add(gtab->pool, (char *) buf_label.data);
	}

	grp->priv = sdef;

	for (i = 0; i < dim; i++)
	{
		offset += strlen(StrPool_get(gtab->pool, offset)) + 1;
		grp->idx[i].i_name = offset;
		offset += strlen(StrPool_get(gtab->pool, offset)) + 1;
		grp->idx[i].i_desc = offset;
	}

	return grp;
}
