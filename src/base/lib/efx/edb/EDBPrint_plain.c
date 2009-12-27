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
	EfiVar *var;
	size_t n;

	for (n = 0, var = type->list; var != NULL; var = var->next)
	{
		char *p = (char *) data + var->offset;

		if	(n)
			n += io_nputc(';', par, 1);

		if	(var->dim)
		{
			n += PrintVecData(par, var->type, p, var->dim);
		}
		else	n += PrintData(par, var->type, p);
	}

	return n + io_nputc('\n', par, 1);
}

static size_t write_sub_list (EfiType *type, void *data, void *par)
{
	EfiVar *var;
	size_t n;

	for (n = 0, var = type->list; var != NULL; var = var->next)
	{
		char *p = (char *) data + var->offset;

		if	(n)
			n += io_nputc(';', par, 1);

		if	(var->dim)
		{
			n += PrintVecData(par, var->type, p, var->dim);
		}
		else if	(var->type->list)
		{
			n += write_sub_list(var->type, p, par);
		}
		else	n += PrintData(par, var->type, p);
	}

	return n;
}

static void init_plain (EDB *edb, EDBPrintMode *mode, IO *io)
{
	if	(mode->header)
	{
		edb_head(edb, io, mode->header > 1);
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
