/*	Header einer multidimensionale Matrix laden
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

static unsigned get_2byte(io_t *io)
{
	int a, b;

	a = io_getc(io);
	b = io_getc(io);

	if	(a != EOF && b != EOF)
		return ((a << 8) + b);

	return 0;
}

static unsigned get_4byte(io_t *io)
{
	unsigned a = get_2byte(io);
	return (a << 16) + get_2byte(io);
}

mdmat_t *md_gethdr(io_t *io)
{
	mdmat_t *md;
	mdaxis_t *x;
	mdaxis_t **ptr;
	char *strbuf;
	size_t j;
	char *p;
	char *oname;
	size_t n;
	size_t dim;
	size_t space;
	size_t recl;

/*	Header laden
*/
	n = get_2byte(io);

	if	(n == MD_OLDMAGIC)
	{
		ushort_t k;

		reg_set(0, io_ident(io));
		errmsg(MSG_MDMAT, 10);
		exit(EXIT_SUCCESS);
		get_LSBF(io, &k, 2);
		dim = k;
		get_LSBF(io, &space, 4);
		get_LSBF(io, &recl, 4);
		(void) get_4byte(io);
	}
	else if	(n == MD_MAGIC)
	{
		dim = get_2byte(io);
		space = get_4byte(io);
		recl = get_4byte(io);
		(void) get_4byte(io);
	}
	else
	{
		liberror(MSG_MDMAT, 11);
		return NULL;
	}

/*	Datenstruktur initialisieren
*/
	md = new_mdmat();
	md->data = NULL;
	ptr = &md->axis;

/*	Indexstruktur initialisieren und Vektordimensionen laden
*/
	for (j = 0; j < dim; j++)
	{
		n = get_4byte(io);
		*ptr = new_axis((size_t) n);
		ptr = &(*ptr)->next;
	}

	*ptr = NULL;

/*	Textteil laden
*/
	strbuf = (char *) memalloc(space);
	io_read(io, strbuf, (size_t) space);
	p = strbuf;
	md->title = mstrcpy(nextstr(&p));
	oname = nextstr(&p);
	md->type = mdtype(oname);

/*	Kompatiblitätsprüfung und Korrektur des Ganzzahldatentypes
*/
	if	(md->type->recl != recl)
	{
		if	(md->type == &Type_long && recl == Type_int.recl)
		{
			md->type = &Type_int;
		}
		else
		{
			reg_cpy(1, oname);
			liberror(MSG_MDMAT, 14);
			return NULL;
		}
	}

	if	(recl == 0)
	{
		reg_cpy(1, oname);
		errmsg(MSG_MDMAT, 17);
	}

/*	Achsen initialisieren
*/
	md->size = md_size(md->axis, md->type->size);

	for (x = md->axis; x != NULL; x = x->next)
	{
		x->name = mstrcpy(nextstr(&p));

		for (j = 0; j < x->dim; j++)
			x->idx[j].name = mstrcpy(nextstr(&p));
	}

	memfree(strbuf);
	return md;
}
