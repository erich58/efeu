/*	Datenwerte in Liste konvertieren
	(c) 1995 Erich Frühstück
*/

#include <EFEU/mdeval.h>
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

static void s_data(void *par, EfiType *type, void *ptr, void *base)
{
	io_putc('\t', par);
	ShowData(par, type, ptr);

	if	(base)
	{
		io_putc('[', par);
		ShowData(par, type, base);
		io_putc(']', par);
	}
}

static MdEvalDef s_eval = {
	NULL, NULL,
	s_newidx, memfree,
	s_start,
	s_end,
	s_data,
};


/*	Datenwerte generieren
*/

static EfiObjList *idx_list = NULL;

static void *d_newidx(const void *idx, char *name)
{
	if	(idx == NULL)	idx = &idx_list;

	*((EfiObjList **) idx) = NewObjList(str2Obj(mstrcpy(name)));
	return &(*((EfiObjList **) idx))->next;
}

static void d_clridx(void *idx)
{
	DelObjList(*((EfiObjList **) idx));
	*((EfiObjList **) idx) = NULL;
}

static void d_start(void *par, const void *idx)
{
	ShowData(iostd, &Type_list, &idx_list);
}


static MdEvalDef d_eval = {
	NULL, NULL,
	d_newidx, d_clridx,
	d_start,
	s_end,
	s_data,
};


void *md_SHOW(mdmat *md, unsigned mask, unsigned base, int lag);
void *md_DATA(mdmat *md, unsigned mask, unsigned base, int lag);

void *md_SHOW(mdmat *md, unsigned mask, unsigned base, int lag)
{
	return md_eval(&s_eval, iostd, md, mask, base, lag);
}

void *md_DATA(mdmat *md, unsigned mask, unsigned base, int lag)
{
	return md_eval(&d_eval, iostd, md, mask, base, lag);
}
