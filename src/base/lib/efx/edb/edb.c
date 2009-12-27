/*
*/


#include <EFEU/EDB.h>
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>

static char *edb_ident (const void *data)
{
	const EDB *edb;
	StrBuf *sb;
	IO *io;

	edb = data;
	sb = sb_create(0);
	io = io_strbuf(sb);

	if	(edb->obj)
	{
		io_puts("(", io);
		PrintType(io, edb->obj->type, 0);
		io_puts(") ", io);
		PrintData(io, edb->obj->type, edb->obj->data);
	}
	else	io_puts("void", io);

	io_close(io);
	return sb2str(sb);
}


static void edb_clean (void *data)
{
	EDB *edb = data;
	edb_closein(edb);
	edb_closeout(edb);
	memfree(edb->desc);
	UnrefObj(edb->obj);
	memfree(edb);
}

static RefType EDB_reftype = REFTYPE_INIT("EDB", edb_ident, edb_clean);

EDB *edb_create (EfiObj *obj, char *desc)
{
	/*
	if	(obj)
	*/
	{
		EDB *edb = memalloc(sizeof *edb);
		edb->desc = desc;
		edb->head = obj ? TypeHead(obj->type) : NULL;
		edb->obj = obj;
		edb->save = 0;
		edb->ipar = NULL;
		edb->read = NULL;
		edb->opar = NULL;
		edb->write = NULL;
		return rd_init(&EDB_reftype, edb);
	}

	return NULL;
}


void edb_head (const EDB *edb, IO *io, int verbosity)
{
	EfiVar *var;

	if	(!edb || !io)	return;

	io_printf(io, "EDB\t1.0\n");

	if	(edb->desc && verbosity)
	{
		char *p;

		io_puts("// ", io);

		for (p = edb->desc; *p != 0; p++)
		{
			if	(*p != '\n')	io_putc(*p, io);
			else if	(p[1])		io_puts("\n// ", io);
			else			break;
		}

		io_putc('\n', io);
	}

	if	(edb->head)
	{
		io_puts("@head\n", io);
		io_puts(edb->head, io);
		io_puts("\n@type\n", io);
	}

	if	(!edb->obj->type->list)
	{
		PrintType(io, edb->obj->type, verbosity ? 2 : 1);
		io_putc('\n', io);
	}

	for (var = edb->obj->type->list; var != NULL; var = var->next)
	{
		PrintType(io, var->type, verbosity ? 2 : 1);
		io_putc(' ', io);
		io_puts(var->name, io);

		if	(var->dim)
			io_printf(io, "[%d]", var->dim);
 
		io_putc(';', io);

		if	(var->desc && verbosity)
			io_printf(io, "\t/* %s */", var->desc);

		io_putc('\n', io);
	}
}

void edb_vlist (const EDB *edb, const char *pfx, IO *io)
{
	EfiVar *var;
	const char *delim;

	if	(!edb || !io)	return;

	delim = pfx;
	var = edb->obj->type->list;

	if	(!var)	return;

	for (var = edb->obj->type->list; var != NULL; var = var->next)
	{
		io_printf(io, "%s%s", delim, var->name);

		if	(var->dim)
			io_printf(io, "[%d]", var->dim);

		delim = ";";
	}

	io_putc('\n', io);
}
