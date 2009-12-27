/*	Testfunktionen für mehrdimensionale Matrizen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>

#define	REF(ptr)	(ref_mdmat(Val_mdmat(ptr)), ptr)

extern void *md_SHOW(mdmat *md, unsigned mask, unsigned base, int lag);
extern void *md_DATA(mdmat *md, unsigned mask, unsigned base, int lag);

static void MF_SHOW(EfiFunc *func, void *rval, void **arg)
{
	md_SHOW(Val_mdmat(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

static void MF_DATA(EfiFunc *func, void *rval, void **arg)
{
	Val_list(rval) = md_DATA(Val_mdmat(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

/*	Achsenflags setzen
*/

static void MF_setflag(EfiFunc *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_mark, Val_int(arg[3]), mdsf_mark, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}

static void MF_setlock(EfiFunc *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_lock, Val_int(arg[3]), mdsf_lock, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}

static void MF_clrflag(EfiFunc *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_clear, Val_int(arg[3]), mdsf_clear, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}


/*	Achsenflags anzeigen
*/

static void MF_showflag(EfiFunc *func, void *rval, void **arg)
{
	unsigned mask;
	mdmat *md;
	mdaxis *x;
	IO *io;
	char *delim;
	int n;

	md = Val_mdmat(arg[0]);
	io = Val_io(arg[1]);
	mask = Val_int(arg[2]);

	if	(md == NULL)
	{
		Val_mdmat(rval) = NULL;
		return;
	}

	for (x = md->axis; x != NULL; x = x->next)
	{
		io_puts(StrPool_get(x->sbuf, x->i_name), io);
		delim = "=";

		if (x->flags)
			io_xprintf(io, "[%x]", x->flags);

		for (n = 0; n < x->dim; n++)
		{
			if (x->idx[n].flags & mask)
				continue;

			io_puts(delim, io);
			delim = ",";
			io_puts(StrPool_get(x->sbuf, x->idx[n].i_name), io);

			if (x->idx[n].flags)
				io_xprintf(io, "[%x]", x->idx[n].flags);
		}

		io_putc('\n', io);
	}

	Val_mdmat(rval) = rd_refer(md);
}


static void MF_cmpaxis(EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = cmp_axis(Val_mdaxis(arg[0]), Val_mdaxis(arg[1]), (int) Val_int(arg[2]));
}

static void show_mdlist (IO *io, mdlist *list)
{
	while (list != NULL)
	{
		int i;

		io_xprintf(io, "name=%#s, opt=%#s, dim=%d\n",
			list->name, list->option, list->dim);

		for (i = 0; i < list->dim; i++)
		{
			io_xprintf(io, "%d:\tlist=%#s, opt=%#s\n",
				i, list->list[i], list->lopt[i]);
		}

		list = list->next;
	}
}

static void MF_mdlist(EfiFunc *func, void *rval, void **arg)
{
	int flag;
	char *p;
	mdlist *list;

	flag = 0;

	if	((p = Val_str(arg[1])))
	{
		if	(strchr(p, 'n'))	flag |= MDLIST_NAMEOPT;
		if	(strchr(p, 'l'))	flag |= MDLIST_LISTOPT;
		if	(strchr(p, 'a'))	flag |= MDLIST_ANYOPT;
		if	(strchr(p, ':'))	flag |= MDLIST_NEWOPT;
	}

	io_puts("flags:", iostd);

	if	(flag & MDLIST_NAMEOPT)	io_puts(" NAMEOPT", iostd);
	if	(flag & MDLIST_LISTOPT)	io_puts(" LISTOPT", iostd);
	if	(flag & MDLIST_NEWOPT)	io_puts(" NEWOPT", iostd);

	io_putc('\n', iostd);
	list = str2mdlist(Val_str(arg[0]), flag);
	show_mdlist(iostd, list);
	del_mdlist(list);
}

static EfiFuncDef fdef[] = {
	{ 0, &Type_mdmat, "mdmat::lock (str, int=0, int=0, int=0)", MF_setlock },
	{ 0, &Type_mdmat, "mdmat::set (str, int=0, int=0, int=0)", MF_setflag },
	{ 0, &Type_mdmat, "mdmat::clr (str, int=0, int=0, int=0)", MF_clrflag },
	{ 0, &Type_mdmat, "mdmat::show (IO io, int=0)", MF_showflag },

	{ 0, &Type_void, "mdmat::SHOW (int=0, int=0, int=0)", MF_SHOW },
	{ 0, &Type_list, "mdmat::DATA (int=0, int=0, int=0)", MF_DATA },

	{ 0, &Type_int, "cmpaxis (mdaxis a, mdaxis b, int flag = 7)",
		MF_cmpaxis },
	{ 0, &Type_void, "mdlist (str def, str flags = \":a\")", MF_mdlist },
};


void SetupMdTest(void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
