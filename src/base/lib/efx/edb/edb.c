/*
*/


#include <EFEU/EDB.h>
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>

static char *edb_ident (const void *data)
{
	const EDB *edb;
	StrBuf *sb;
	char *p;
	IO *io;

	edb = data;
	sb = sb_acquire();
	io = io_strbuf(sb);

	if	(edb->obj)
	{
		PrintType(io, edb->obj->type, 0);
	}
	else	io_puts("void", io);

	if	((p = rd_ident(edb->ipar)))
	{
		io_puts(" in=", io);
		io_puts(p, io);
		memfree(p);
	}

	if	((p = rd_ident(edb->opar)))
	{
		io_puts(" out=", io);
		io_puts(p, io);
		memfree(p);
	}

	io_close(io);
	return sb_cpyrelease(sb);
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

EDB *edb_alloc (EfiObj *obj, char *desc)
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
		edb->nread = 0;
		edb->opar = NULL;
		edb->write = NULL;
		edb->nwrite = 0;
		return rd_init(&EDB_reftype, edb);
	}

	return NULL;
}

EDB *edb_create (EfiType *type)
{
	return edb_alloc(LvalObj(NULL, type), NULL);
}

EDB *edb_share (EDB *base)
{
	return edb_alloc(RefObj(base->obj), mstrcpy(base->desc));
}

void edb_head (const EDB *edb, IO *io, int verbosity)
{
	EfiStruct *var;

	if	(!edb || !io)	return;

	io_xprintf(io, "EDB\t1.0\n");

	if	(edb->desc && verbosity > 1)
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

	if	(edb->obj->type->cname || !edb->obj->type->list)
	{
		PrintType(io, edb->obj->type, verbosity);
		io_putc('\n', io);
		return;
	}

	for (var = edb->obj->type->list; var != NULL; var = var->next)
	{
		PrintType(io, var->type, verbosity);
		io_putc(' ', io);
		io_puts(var->name, io);

		if	(var->dim)
			io_xprintf(io, "[%d]", var->dim);
 
		io_putc(';', io);

		if	(var->desc && verbosity > 1)
			io_xprintf(io, "\t/* %s */", var->desc);

		io_putc('\n', io);
	}
}

void edb_vlist (const EDB *edb, const char *pfx, IO *io)
{
	EfiStruct *var;
	const char *delim;

	if	(!edb || !io)	return;

	delim = pfx;
	var = edb->obj->type->list;

	if	(!var)	return;

	for (var = edb->obj->type->list; var != NULL; var = var->next)
	{
		io_xprintf(io, "%s%s", delim, var->name);

		if	(var->dim)
			io_xprintf(io, "[%d]", var->dim);

		delim = ";";
	}

	io_putc('\n', io);
}
