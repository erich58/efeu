/*	Datenausgabe im Textformat
*/

#include <EFEU/EDB.h>
#include <EFEU/printobj.h>

static size_t write_vec (EfiType *type, void *data, void *par)
{
	size_t n = PrintVecData(par, type->base, data, type->dim);
	return n + io_nputc('\n', par, 1);
}

static size_t write_label (EfiType *type, void *data, void *par)
{
	size_t n = ShowData(par, type, data);
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
		else	n += ShowData(par, var->type, p);
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
		else	n += ShowData(par, var->type, p);
	}

	return n;
}

static size_t write_xlist (EfiType *type, void *data, void *par)
{
	size_t n = write_sub_list(type, data, par);
	return n + io_nputc('\n', par, 1);
}

static void init_label (EDB *edb, EDBPrintMode *mode, IO *io)
{
	if	(mode->header)
	{
		edb_head(edb, io, mode->header > 1);
		io_puts("@data label\n", io);
	}

	edb_vlist(edb, "#", io);

	if	(edb->obj->type->list)	edb->write = write_list;
	else if	(edb->obj->type->dim)	edb->write = write_vec;
	else				edb->write = write_label;

	edb->opar = io;
}

void EDBPrint_label (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->name = def->name;
	mode->init = init_label;
}

static void head_xlist (EfiType *type, const char *name, IO *io)
{
	EfiVar *var;
	int flag = 0;

	for (var = type->list; var != NULL; var = var->next)
	{
		char *p = mstrpaste(".", name, var->name);

		if	(flag++)
			io_putc(';', io);

		if	(var->dim)
		{
			io_printf(io, "%s[%d]", p, var->dim);
		}
		else if	(var->type->list)
		{
			head_xlist(var->type, p, io);
		}
		else	io_printf(io, "%s", p);

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
	mode->init = init_data;

	if	(mstrcmp("crlf", arg) == 0)
		mode->filter = io_crlf;
}
