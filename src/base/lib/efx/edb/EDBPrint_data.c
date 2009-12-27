/*	Datenausgabe im Textformat
*/

#include <EFEU/EDB.h>
#include <EFEU/printobj.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiOutput.h>

#define	ERR_PRT	"[edb:print]$!: type $1: undefined print method $2.\n"

void EDBPrint_data_init (EDB *edb, EDBPrintMode *mode, IO *io)
{
	EfiType *type;
	AssignArg *xarg;
	EfiOutput *pdef;

	type = edb->obj->type;
	xarg = rd_data(mode->par);

	if	(!xarg || !xarg->name[0])
	{
		ListEfiPar(ioerr, type, &EfiPar_output, NULL, 0);
		exit(EXIT_SUCCESS);
	}

	pdef = SearchEfiPar(type, &EfiPar_output, xarg->name);

	if	(!pdef)
		log_error(edb_err,  ERR_PRT, "ss", type->name, xarg->name);

	if	(xarg->arg && xarg->arg[0] == '?')
	{
		PrintEfiPar(ioerr, pdef);
		exit(EXIT_SUCCESS);
	}

	edb->write = pdef->out_write;
	edb->opar = pdef->out_open ? pdef->out_open(io, type, pdef->par,
		xarg->opt, xarg->arg) : NULL;
}


static size_t write_label (EfiType *type, void *data, void *par)
{
	size_t n = ShowData(par, type, data);
	return n + io_nputc('\n', par, 1);
}

static size_t write_obj (EfiObj *base, IO *out)
{
	EfiStruct *var;
	EfiObj *obj;
	size_t n;

	for (n = 0, var = base->type->list; var != NULL; var = var->next)
	{
		if	(n)
			n += io_nputc(';', out, 1);

		obj = Var2Obj(var, base);

		if	(obj && obj->type->list)
			n += write_obj(obj, out);
		else	n += ShowObj(out, obj);

		UnrefObj(obj);
	}

	return n;
}

static size_t write_xlist (EfiType *type, void *data, void *par)
{
	EfiObj *base = LvalObj(&Lval_ptr, type, data);
	size_t n = write_obj(base, par);
	UnrefObj(base);
	return n + io_nputc('\n', par, 1);
}


static void head_xlist (EfiType *type, const char *name, IO *io)
{
	EfiStruct *var;
	int flag = 0;

	for (var = type->list; var != NULL; var = var->next)
	{
		char *p = mstrpaste(".", name, var->name);

		if	(flag++)
			io_putc(';', io);

		if	(var->dim)
		{
			io_xprintf(io, "%s[%d]", p, var->dim);
		}
		else if	(var->type->list)
		{
			head_xlist(var->type, p, io);
		}
		else	io_xprintf(io, "%s", p);

		memfree(p);
	}
}

static void init_data (EDB *edb, EDBPrintMode *mode, IO *io)
{
	EfiType *type = edb->obj->type;

	if	(mode->header && type->list)
	{
		head_xlist(type, NULL, io);
		io_putc('\n', io);
	}

	edb->write = type->list ? write_xlist : write_label;
	edb->opar = io;
}

void EDBPrint_data (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;

	if	(mstrcmp("crlf", opt) == 0)
		mode->filter = io_crlf;

	if	(arg)
	{
		mode->init = EDBPrint_data_init;
		mode->par = rd_wrap(assignarg(arg, NULL, NULL), memfree);
	}
	else
	{
		mode->init = init_data;
		mode->par = NULL;
	}
}
