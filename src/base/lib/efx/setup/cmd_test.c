/*	Programmsteuerung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/cmdsetup.h>
#include <EFEU/parsedef.h>
#include <EFEU/konvobj.h>
#include <EFEU/locale.h>


static void list_vtab(io_t *io, VarTab_t *tab, int limit)
{
	if	(tab == NULL || tab->tab.dim <= limit)
		ShowVarTab(io, NULL, tab);
	else if	(tab->name)
		io_printf(io, "%s[%d]", tab->name, tab->tab.dim);
	else	io_printf(io, "%#p[%d]", tab, tab->tab.dim);
}

static void f_vtabstack (Func_t *func, void *rval, void **arg)
{
	VarStack_t *stack; 
	io_t *io;
	int limit;

	limit = Val_int(arg[0]);
	io = Val_io(arg[1]);
	list_vtab(io, LocalVar, limit);

	for (stack = VarStack; stack != NULL; stack = stack->next)
	{
		io_puts(", ", io);
		list_vtab(io, stack->tab, limit);
	}

	io_putc('\n', io);
}


static void f_typedist (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = KonvDist(Val_type(arg[0]), Val_type(arg[1]));
}


static struct {
	int flag;
	char *name;
} dist_tab[] = {
	{ D_MAXDIST, "MAXDIST" },
	{ D_CREATE, "CREATE" },
	{ D_RESTRICTED, "RESTRICTED" },
	{ D_VAARG, "VAARG" },
	{ D_ACCEPT, "ACCEPT" },
	{ D_BASE, "BASE" },
	{ D_KONVERT, "KONVERT" },
	{ D_EXPAND, "EXPAND" },
	{ D_PROMOTE, "PROMOTE" },
};

static void f_showkonv (Func_t *func, void *rval, void **arg)
{
	Type_t *old, *new;
	Konv_t konv;

	old = Val_type(arg[0]);
	new = Val_type(arg[1]);

	if	(old == NULL || new == NULL)
		return;

	io_printf(iostd, "%s -> %s: ", old->name, new->name);

	if	(GetKonv(&konv, old, new))
	{
		char *delim;
		int i;

		io_printf(iostd, "%#4o", konv.dist);
		delim = " ";

		for (i = 0; i < tabsize(dist_tab); i++)
		{
			if	(konv.dist & dist_tab[i].flag)
			{
				io_puts(delim, iostd);
				io_puts(dist_tab[i].name, iostd);
				delim = "|";
			}
		}

		io_putc('\n', iostd);
	}
	else	io_puts("-1 REJECT\n", iostd);
}

static void f_lcshow (Func_t *func, void *rval, void **arg)
{
	io_t *io;
	int n;

	io = Val_io(arg[0]);
	n = 0;

	if	(Locale.scan)
	{
		n += io_printf(io, "scan.thousands_sep = %#s\n",
			Locale.scan->thousands_sep);
		n += io_printf(io, "scan.decimal_point = %#s\n",
			Locale.scan->decimal_point);
	}

	if	(Locale.print)
	{
		n += io_printf(io, "print.thousands_sep = %#s\n",
			Locale.print->thousands_sep);
		n += io_printf(io, "print.decimal_point = %#s\n",
			Locale.print->decimal_point);
		n += io_printf(io, "print.negativ_sign = %#s\n",
			Locale.print->negative_sign);
		n += io_printf(io, "print.positive_sign = %#s\n",
			Locale.print->positive_sign);
		n += io_printf(io, "print.zero_sign = %#s\n",
			Locale.print->zero_sign);
	}

	Val_int(rval) = n;
}

static void f_parselist (Func_t *func, void *rval, void **arg)
{
	ListParseDef(Val_io(arg[0]));
}

/*	Initialisierung
*/

void CmdSetup_test(void)
{
	SetFunc(0, &Type_void, "vtabstack (int = 0, IO = iostd)", f_vtabstack);
	SetFunc(0, &Type_int, "dist (Type_t a, Type_t b)", f_typedist);
	SetFunc(0, &Type_void, "showkonv (Type_t a, Type_t b)", f_showkonv);
	SetFunc(0, &Type_int, "lcshow (IO = iostd)", f_lcshow);
	SetFunc(0, &Type_void, "parselist (IO = iostd)", f_parselist);
}
