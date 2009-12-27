/*	Sortierte Ausgabe einer multidimensionale Matrix
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2

Das Element flags in der mdidx_t - Struktur wird als Datenindex
verwendet. Damit kann der index - Vektor mit qsort sortiert werden.
Bei der Ausgabe ist der Pointer entsprechend dem Wert von flags zu
setzen. Achsen mit Nullpointer als Bezeichner werden gelöscht.
*/

#include <EFEU/stfunc.h>
#include <EFEU/mdmat.h>

static void putdata (io_t *io, mdaxis_t *x, void *data, StFunc_t *st);
static int std_cmp (mdidx_t *a, mdidx_t *b);
static int test (mdaxis_t *x, int idx);


/*	Standardvergleichsfunktion
*/

static int std_cmp(mdidx_t *a, mdidx_t *b)
{
	return strcmp(a->name, b->name);
}


/*	Testfunktion zur Achsenunterdrückung
*/

static int test(mdaxis_t *x, int n)
{
	return (x->idx[n].name != NULL);
}


void md_ssave(io_t *io, mdmat_t *md, int (*cmp) (mdidx_t *, mdidx_t *), char **list, size_t dim)
{
	mdaxis_t *x;
	StFunc_t *st;
	ulong_t *ptr;
	int i;

	if	(cmp == NULL)	cmp = std_cmp;

	for (x = md->axis; x != NULL; x = x->next)
	{
		ptr = ALLOC(x->dim, ulong_t);

		for (i = 0; i < x->dim; i++)
			x->idx[i].flags = i;

		if	(!patselect(x->name, list, dim))
			continue;

		qsort(x->idx, x->dim, sizeof(mdidx_t), (comp_t) cmp);
	}

	md_puthdr(io, md, 0);
	st = VaGetStFunc("fwrite", 2, md->type, 0, &Type_io, 0);
	putdata(io, md->axis, md->data, st);
	DelStFunc(st);
	md_puteof(io);
}


/*	Daten ausgeben
*/

static void putdata(io_t *io, mdaxis_t *x, void *data, StFunc_t *st)
{
	if	(x != NULL)
	{
		size_t i;

		for (i = 0; i < x->dim; i++)
		{
			if	(!test(x, i))	continue;

			putdata(io, x->next, ((char *) data) +
				x->idx[i].flags * x->size, st);
		}
	}
	else	CallStFunc(st, NULL, NULL, data, &io);
}
