/*	EFEU-Parameter
	$Copyright (C) 2006 Erich Frühstück
	A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EfiPar.h>

/*	Globale Moduldefinitionen
*/

static VECBUF(epc_buf, 64, sizeof(EfiParClass *));

static int epc_cmp (const void *a, const void *b)
{
	EfiParClass **ka = (EfiParClass **) a;
	EfiParClass **kb = (EfiParClass **) b;

	if	((*ka)->epc_par != (*kb)->epc_par)
		return mstrcmp((*ka)->epc_par->name, (*kb)->epc_par->name);

	return mstrcmp((*ka)->epc_name, (*kb)->epc_name);
}

static VecBuf *epc_ptr (EfiType *type)
{
	return type ? &type->par : &epc_buf;
}

void *SearchEfiPar (EfiType *type, EfiPar *par, const char *name)
{
	EfiParClass key, *kp, **rp;
	EfiType *base;

	if	(!name || !*name)	return NULL;

	if	(mstrcmp(name, "?") == 0)
	{
		ListEfiPar(ioerr, type, par, NULL, 0);
		exit(EXIT_SUCCESS);
	}

	key.epc_par = par;
	key.epc_name = name;
	kp = &key;

	for (base = type; base; base = base->base)
	{
		rp = vb_search(&base->par, &kp, epc_cmp, VB_SEARCH);

		if	(rp && (!(*rp)->epc_type || (*rp)->epc_type == type))
			return *rp;
	}

	rp = vb_search(&epc_buf, &kp, epc_cmp, VB_SEARCH);
	return rp ? *rp : NULL;
}

void *GetEfiPar (EfiType *type, EfiPar *par, const char *name)
{
	EfiParClass key, *kp, **rp;
	key.epc_par = par;
	key.epc_name = name;
	kp = &key;
	rp = vb_search(epc_ptr(type), &kp, epc_cmp, VB_SEARCH);
	return rp ? *rp : NULL;
}

void AddEfiPar (EfiType *type, void *data)
{
	vb_search(epc_ptr(type), &data, epc_cmp, VB_REPLACE);
}

void AddEfiParTab (EfiType *type, void *data, size_t dim, size_t size)
{
	VecBuf *buf;
	char *ptr;

	buf = epc_ptr(type);

	for (ptr = data; dim-- > 0; ptr += size)
	{
		data = ptr;
		vb_search(buf, &data, epc_cmp, VB_REPLACE);
	}
}

/*	Modultabelle durchwandern
*/

static void epc_walk (VecBuf *buf, EfiPar *par,
	void (*visit) (EfiParClass *entry, void *data), void *data)
{
	EfiParClass **ptr;
	size_t dim;

	for (dim = buf->used, ptr = buf->data; dim-- > 0; ptr++)
		if (par == NULL || (*ptr)->epc_par == par)
			visit(*ptr, data);
}

void EfiParWalk (EfiType *type, EfiPar *par,
	void (*visit) (EfiParClass *entry, void *data), void *data)
{
	if	(type)
		epc_walk(&type->par, par, visit, data);

	if	(par || !type)
		epc_walk(&epc_buf, par, visit, data);
}

/*	Parameterinformationen ausgeben
*/

void PrintEfiPar (IO *io, void *data)
{
	EfiParClass *epc = data;

	if	(epc->epc_info)
	{
		epc->epc_info(io, data);
		return;
	}

	io_xprintf(io, "%s\n\t%s\n", epc->epc_name, epc->epc_label);
}

#define	LIST_WIDTH	13
#define	LIST_BREAK	65

static void list_modul (IO *io, VecBuf *buf, EfiPar *par,
	const char *pfx, int mode)
{
	const EfiPar *last;
	EfiParClass **ptr;
	size_t idx;
	int col;

	last = NULL;
	ptr = buf->data;
	idx = buf->used;

	for (col = 0; idx-- > 0; ptr++)
	{
		if	(par)
		{
			if	((*ptr)->epc_par != par)
				continue;
		}
		else if	(last != (*ptr)->epc_par)
		{
			if	(col)
			{
				io_putc('\n', io);
				col = 0;
			}

			last = (*ptr)->epc_par;
			io_xprintf(io, "\n%s\n\n", last->label);
		}

		if	(mode == 0)
		{
			if	(col > LIST_BREAK)
			{
				io_putc('\n', io);
				col = 0;
			}
			else if	(col)
			{
				col += io_nputc(' ', io, LIST_WIDTH *
					(col / LIST_WIDTH + 1) - col);
			}

			if	(pfx)
			{
				col += io_puts(pfx, io);
				col += io_nputc('.', io, 1);
			}

			col += io_puts((*ptr)->epc_name, io);
		}
		else if	(mode == 1)
		{
			if	(pfx)
				io_xprintf(io, "%s.", pfx);

			io_xprintf(io, "%s\t%s\n", (*ptr)->epc_name,
				(*ptr)->epc_label);
		}
		else	PrintEfiPar(io, *ptr);
	}

	if	(col)
		io_putc('\n', io);
}

void ListEfiPar (IO *io, EfiType *type, EfiPar *par,
	const char *pfx, int verbosity)
{
	EfiType *base;

	if	(!io)	return;

	for (base = type; base; base = base->base)
	{
		list_modul(io, &base->par, par, pfx, verbosity);

		if	(!par)	break;
	}

	if	(par || !type)
		list_modul(io, &epc_buf, par, pfx, verbosity);
}
