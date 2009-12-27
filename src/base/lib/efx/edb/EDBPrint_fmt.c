/*	Formatierte Ausgabe von Datenzeilen
*/

#include <EFEU/EDB.h>
#include <EFEU/printobj.h>

typedef struct {
	REFVAR;	/* Referenzvariablen */
	char *head;
	char *line;
	IO *io;
} PARG;

static void parg_clean (void *data)
{
	PARG *parg = data;

	memfree(parg->head);
	memfree(parg->line);
	io_close(parg->io);
	memfree(parg);
}

static RefType parg_reftype = REFTYPE_INIT("EDBPrint_fmt", NULL, parg_clean);

static PARG *parg_alloc (const char *opt, const char *arg)
{
	PARG *parg = memalloc(sizeof *parg);
	parg->head = mstrcpy(opt);
	parg->line = mstrcpy(arg);
	parg->io = NULL;
	return rd_init(&parg_reftype, parg);
}

static size_t write_fmt (EfiType *type, void *data, void *par)
{
	PARG *parg = par;
	int n;
	
	PushVarTab(RefVarTab(type->vtab), LvalObj(&Lval_ptr, type, data));
	n = io_psubvec(parg->io, parg->line, 0, NULL);
	PopVarTab();
	return n + io_nputc('\n', parg->io, 1);
}

static void init_fmt (EDB *edb, EDBPrintMode *mode, IO *io)
{
	PARG *arg = mode->par;

	if	(arg->head)
	{
		io_psubarg(io, arg->head, "ns", edb->desc);
		io_putc('\n', io);
	}

	edb->write = write_fmt;
	arg->io = io;
	edb->opar = arg;
	mode->par = NULL;
}

void EDBPrint_fmt (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg)
{
	mode->par = parg_alloc(opt, arg);
	mode->name = def->name;
	mode->init = init_fmt;
	mode->split = 0;
}
