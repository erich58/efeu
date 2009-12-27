/*	Testfunktionen für mehrdimensionale Matrizen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>

#define	REF(ptr)	(ref_mdmat(Val_mdmat(ptr)), ptr)

extern void *md_SHOW(mdmat_t *md, unsigned mask, unsigned base, int lag);
extern void *md_DATA(mdmat_t *md, unsigned mask, unsigned base, int lag);

static void MF_SHOW(Func_t *func, void *rval, void **arg)
{
	md_SHOW(Val_mdmat(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

static void MF_DATA(Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = md_DATA(Val_mdmat(arg[0]), Val_int(arg[1]),
		Val_int(arg[2]), Val_int(arg[3]));
}

/*	Achsenflags setzen
*/

static void MF_setflag(Func_t *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_mark, Val_int(arg[3]), mdsf_mark, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}

static void MF_setlock(Func_t *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_lock, Val_int(arg[3]), mdsf_lock, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}

static void MF_clrflag(Func_t *func, void *rval, void **arg)
{
	md_setflag(Val_mdmat(arg[0]), Val_str(arg[1]), Val_int(arg[2]),
		mdsf_clear, Val_int(arg[3]), mdsf_clear, Val_int(arg[4]));
	Val_mdmat(rval) = rd_refer(Val_mdmat(arg[0]));
}


/*	Achsenflags anzeigen
*/

static void MF_showflag(Func_t *func, void *rval, void **arg)
{
	unsigned mask;
	mdmat_t *md;
	mdaxis_t *x;
	io_t *io;
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
		io_puts(x->name, io);
		delim = "=";

		if (x->flags) io_printf(io, "[%x]", x->flags);

		for (n = 0; n < x->dim; n++)
		{
			if (x->idx[n].flags & mask) continue;

			io_puts(delim, io);
			delim = ",";
			io_puts(x->idx[n].name, io);

			if (x->idx[n].flags) io_printf(io, "[%x]", x->idx[n].flags);
		}

		io_putc('\n', io);
	}

	Val_mdmat(rval) = rd_refer(md);
}


static void MF_cmpaxis(Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = cmp_axis(Val_mdaxis(arg[0]), Val_mdaxis(arg[1]), (int) Val_int(arg[2]));
}


static FuncDef_t fdef[] = {
	{ 0, &Type_mdmat, "mdmat::lock (str, int=0, int=0, int=0)", MF_setlock },
	{ 0, &Type_mdmat, "mdmat::set (str, int=0, int=0, int=0)", MF_setflag },
	{ 0, &Type_mdmat, "mdmat::clr (str, int=0, int=0, int=0)", MF_clrflag },
	{ 0, &Type_mdmat, "mdmat::show (IO io, int=0)", MF_showflag },

	{ 0, &Type_void, "mdmat::SHOW (int=0, int=0, int=0)", MF_SHOW },
	{ 0, &Type_list, "mdmat::DATA (int=0, int=0, int=0)", MF_DATA },

	{ 0, &Type_int, "cmpaxis (mdaxis a, mdaxis b, int flag = 7)", MF_cmpaxis },
};


void SetupMdTest(void)
{
	AddFuncDef(fdef, tabsize(fdef));
}
