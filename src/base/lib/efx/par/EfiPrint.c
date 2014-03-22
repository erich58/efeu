/*	EFEU-Ausgabefunktionen
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EfiOutput.h>

typedef struct {
	REFVAR;	/* Referenzvariablen */
	int (*func) (IO *io, void *data, unsigned flags);
	int flags;
	IO *io;
} PARG;

static void parg_clean (void *data)
{
	PARG *parg = data;
	io_close(parg->io);
	memfree(parg);
}

static RefType parg_reftype = REFTYPE_INIT("EfiPrintPar", NULL, parg_clean);

void EfiPrint_list (EfiPrintDef *pdef, IO *io, const char *fmt)
{
	int i;

	for (i = 0; i < pdef->fdim; i++)
		io_xprintf(io, fmt, pdef->ftab[i].key, pdef->ftab[i].desc);
}

void EfiPrint_info (IO *io, const void *data)
{
	const EfiOutput *epc = data;
	EfiPrintDef *pdef = epc->par;

	io_puts(epc->epc_name, io);

	if	(pdef->fdim)
	{
		io_putc('=', io);
		EfiPrint_list(pdef, io, "%c");
	}

	io_putc('\t', io);
	io_puts(epc->epc_label, io);
	io_putc('\n', io);
	EfiPrint_list(pdef, io, "\t%c\t%s\n");
}

unsigned EfiPrint_flag (EfiPrintDef *pdef, const char *def)
{
	unsigned val;
	int i;

	if	(def == NULL || pdef->fdim == 0)
		return 0;

	for (val = 0; *def != 0; def++)
	{
		for (i = 0; i < pdef->fdim; i++)
			if (pdef->ftab[i].key == *def)
				val |= pdef->ftab[i].flag;
	}

	return val;
}

void *EfiPrint_open (IO *out, const EfiType *type,
	void *par, const char *opt, const char *arg)
{
	EfiPrintDef *pdef;
	PARG *parg;

	pdef = par;
	parg = memalloc(sizeof *parg);
	parg->func = pdef->func;
	parg->flags = EfiPrint_flag(pdef, arg);
	parg->io = out;
	return rd_init(&parg_reftype, parg);
}

size_t EfiPrint_write (EfiType *type, void *data, void *par)
{
	PARG *parg = par;
	return parg->func(parg->io, data, parg->flags);
}
