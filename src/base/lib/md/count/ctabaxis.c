/*	Zählerstruktur generieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>


static Label_t glabel = { "GES", "Insgesamt" };

static MdClass_t gsel = {
	"GES",		/* name */
	"Insgesamt",	/* desc */
	1,		/* dim */
	&glabel,	/* label */
	NULL,		/* classify */
};


static ALLOCTAB(sdef_tab, 16, sizeof(cgrp_t));
static MdClass_t *mksubsel (const char *def, MdClass_t *s);
static int subcopy (io_t *in, io_t *out, int key,
	const char *arg, unsigned flags);


static iocpy_t c_index[] = {
	{ "/", NULL, 1, iocpy_cskip },
	{ "\\", "!", 1, iocpy_esc },
	{ "[", NULL, 1, subcopy },
	{ "%s;]", NULL, 0, NULL },
	{ "=", NULL, 0, NULL },
};

#define	NAME_DIM	5
#define	LIST_DIM	4


static iocpy_t c_sub[] = {
	{ "/", NULL, 1, iocpy_cskip },
	{ "\\", "!", 1, iocpy_esc },
	{ "[", NULL, 1, subcopy },
	{ "]", NULL, 0, NULL },
};


/*	Eintrag initialisieren
*/

mdaxis_t *md_classaxis(const char *name, ...)
{
	va_list list;
	MdClass_t *class;
	mdaxis_t *grp;
	cgrp_t *sdef, **x;
	size_t dim;
	io_t *tmp;
	int i, j;

	if	(name == NULL)	return NULL;

	sdef = NULL;
	x = &sdef;
	dim = 0;
	tmp = io_tmpbuf(0);
	io_putstr(name, tmp);

	list = va_start(list, name);

	while ((class = va_arg(list, MdClass_t *)) != NULL)
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

mdaxis_t *md_ctabaxis(io_t *io, void *gtab)
{
	mdaxis_t *grp;
	cgrp_t *sdef, **x;
	size_t dim;
	mdlist_t *def;
	io_t *tmp = NULL;
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
		MdClass_t *s;

		s = (MdClass_t *) skey_find(gtab, def->list[i]);

		if	(s == NULL || s->dim == 0)
		{
			reg_set(1, def->list[i]);
			errmsg(MSG_MDMAT, 44);
			continue;
		}

		s = mksubsel(def->lopt[i], s);

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



/*	Unterselektion generieren
*/

static VECBUF(buf_label, 64, sizeof(Label_t));

static MdClass_t *mksubsel(const char *def, MdClass_t *main)
{
	int c;
	int i;
	char *p;
	char **list;
	int dim;
	MdSubClass_t *sub;
	Label_t *label;
	io_t *io;

	if	(def == NULL)	return main;

	io = io_cstr(def);
	sub = MdSubClass(main);
	buf_label.used = 0;

	while ((c = io_eat(io, "%s;")) != EOF)
	{
		p = miocpy(io, c_index, NAME_DIM);
		label = NULL;

		if	(iocpy_last == '=')
		{
			(void) io_getc(io);
			label = vb_next(&buf_label);
			label->name = p;
			label->desc = NULL;
			sub->dim++;
			p = miocpy(io, c_index, LIST_DIM);
		}

		dim = strsplit(p, ",", &list);
		FREE(p);

		for (i = 0; i < main->dim; i++)
		{
			if	(sub->idx[i] < main->dim)
			{
				continue;
			}
			else if	(!patselect(main->label[i].name, list, dim))
			{
				continue;
			}
			else if	(label)
			{
				sub->idx[i] = sub->dim - 1;
				p = label->desc;
				label->desc = mstrpaste("; ", p,
					main->label[i].desc);
				memfree(p);
			}
			else
			{
				label = vb_next(&buf_label);
				label->name = mstrcpy(main->label[i].name);
				label->desc = mstrcpy(main->label[i].desc);
				sub->idx[i] = sub->dim++;
			}
		}

		FREE(list);
	}

	io_close(io);
	sub->label = memalloc(sub->dim * sizeof(Label_t));
	memcpy(sub->label, buf_label.data, sub->dim * sizeof(Label_t));
	return (MdClass_t *) sub;
}


static int subcopy(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	int n;

	io_putc(c, out);
	n = 1 + iocpy(in, out, c_sub, tabsize(c_sub));

	if	(iocpy_last != EOF)
	{
		c = io_getc(in);
		io_putc(c, out);
		n++;
	}

	return n;
}
