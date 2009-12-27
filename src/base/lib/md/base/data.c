/*	Datenwerte in Liste konvertieren
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/mdmat.h>
#include <EFEU/printobj.h>

/*	Datenwerte ausgeben
*/

static void *s_newidx(const void *idx, char *name)
{
	return mstrpaste(".", idx, name);
}

static void s_start(void *par, const void *idx)
{
	io_puts(idx, par);
}

static void s_end(void *par)
{
	io_putc('\n', par);
}

static void s_data(void *par, Type_t *type, void *ptr, void *base)
{
	io_putc('\t', par);
	PrintData(par, type, ptr);

	if	(base)
	{
		io_putc('[', par);
		PrintData(par, type, base);
		io_putc(']', par);
	}
}

static mdeval_t s_eval = {
	NULL, NULL,
	s_newidx, memfree,
	s_start,
	s_end,
	s_data,
};


/*	Datenwerte generieren
*/

static ObjList_t *idx_list = NULL;

static void *d_newidx(const void *idx, char *name)
{
	if	(idx == NULL)	idx = &idx_list;

	*((ObjList_t **) idx) = NewObjList(str2Obj(mstrcpy(name)));
	return &(*((ObjList_t **) idx))->next;
}

static void d_clridx(void *idx)
{
	DelObjList(*((ObjList_t **) idx));
	*((ObjList_t **) idx) = NULL;
}

static void d_start(void *par, const void *idx)
{
	PrintData(iostd, &Type_list, &idx_list);
}


static mdeval_t d_eval = {
	NULL, NULL,
	d_newidx, d_clridx,
	d_start,
	s_end,
	s_data,
};


void *md_SHOW(mdmat_t *md, unsigned mask, unsigned base, int lag);
void *md_DATA(mdmat_t *md, unsigned mask, unsigned base, int lag);

void *md_SHOW(mdmat_t *md, unsigned mask, unsigned base, int lag)
{
	return md_eval(&s_eval, iostd, md, mask, base, lag);
}

void *md_DATA(mdmat_t *md, unsigned mask, unsigned base, int lag)
{
	return md_eval(&d_eval, iostd, md, mask, base, lag);
}
