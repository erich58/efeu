/*	Datenausgabe im Textformat
*/

#include <EFEU/EDB.h>
#include <EFEU/printobj.h>

static size_t write_vec (EfiType *type, void *data, void *par)
{
	size_t n = PrintVecData(par, type->base, data, type->dim);
	return n + io_nputc('\n', par, 1);
}

static size_t write_plain (EfiType *type, void *data, void *par)
{
	size_t n = PrintData(par, type, data);
	return n + io_nputc('\n', par, 1);
}

static size_t write_list (EfiType *type, void *data, void *par)
{
	EfiStruct *var;
	EfiObj *obj, *base;
	size_t n;

	base = LvalObj(&Lval_ptr, type, data);

	for (n = 0, var = type->list; var != NULL; var = var->next)
	{
		if	(n)
			n += io_nputc(';', par, 1);

		obj = Var2Obj(var, base);
		n += PrintObj(par, obj);
		UnrefObj(obj);
	}

	UnrefObj(base);
	return n + io_nputc('\n', par, 1);
}


static void init_plain (EDB *edb, EDBPrintMode *mode, IO *io)
{
	if	(mode->header)
	{
		edb_head(edb, io, mode->header);
		io_puts("@data plain\n", io);
	}

	edb_vlist(edb, "#", io);

	if	(edb->obj->type->list)	edb->write = write_list;
	else if	(edb->obj->type->dim)	edb->write = write_vec;
	else				edb->write = write_plain;

	edb->opar = io;
}

void EDBPrint_plain (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_plain;
}
