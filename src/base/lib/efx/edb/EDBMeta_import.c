/*
Liste externer Dateien importieren
$Copyright (C) 2006 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDBMeta.h>
#include <EFEU/parsearg.h>
#include <EFEU/EfiInput.h>

#define	ERR_DATA "[edb:input]$!: type $1: undefined input method $2.\n"

typedef struct XCATPAR {
	REFVAR;
	EfiInput *idef;
	char *path;
	void *par;
	char *opt;
	char *arg;
	char **tab;
	size_t dim;
} XCATPAR;

static void xcat_clean (void *data)
{
	XCATPAR *cat = data;
	int i;

	for (i = 0; i < cat->dim; i++)
		memfree(cat->tab[i]);

	rd_deref(cat->par);
	memfree(cat->opt);
	memfree(cat->arg);
	memfree(cat);
}

static RefType xcat_reftype = REFTYPE_INIT("EDB_IMPORT", NULL, xcat_clean);

static IO *data_open (const char *path, const char *name)
{
	if	(*name == 0)
	{
		return NULL;
	}
	else if	(strcmp("-", name) == 0)
	{
		return io_fileopen(NULL, "rz");
	}
	else if	(strchr("|&./", *name))
	{
		return io_fileopen(name, "rz");
	}
	else	return io_findopen(path, name, NULL, "rz");
}

static int xcat_read (EfiType *type, void *data, void *par)
{
	XCATPAR *cat = par;

	for (;;)
	{
		if	(cat->idef->in_read(type, data, cat->par))
			return 1;

		rd_deref(cat->par);

		if	(cat->dim)
		{
			IO *io = data_open(cat->path, cat->tab[0]);
			cat->par = cat->idef->in_open ?
				cat->idef->in_open(io, type,
				cat->idef->par, cat->opt, cat->arg) : io;
			memfree(cat->tab[0]);
			cat->tab++;
			cat->dim--;
		}
		else
		{
			cat->par = NULL;
			break;
		}
	}

	return 0;
}

void EDBMeta_import (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	AssignArg *mode;
	EfiInput *idef;
	EfiType *type;
	IO *io;
	void *par;
	int i, n;

	mode = assignarg(arg, NULL, NULL);
	type = meta->cur->obj->type;
	idef = mode ? SearchEfiPar(type, &EfiPar_input, mode->name)
		: &EfiInput_plain;

	if	(idef)
	{
		if	(mode && mode->arg && mode && mode->arg[0] == '?')
		{
			PrintEfiPar(ioerr, idef);
			memfree(mode);
			exit(EXIT_SUCCESS);
		}
	}
	else
	{
		dbg_error("edb",  ERR_DATA, "ms", Type2str(type), mode->name);
		memfree(mode);
		return;
	}

	n = EDBMeta_list(meta);

	if	(n <= 0)
	{
		memfree(mode);
		return;
	}

	io = data_open(meta->path, EDBMeta_next(meta));
	par = idef->in_open ? idef->in_open(io, type,
		idef->par, mode->opt, mode->arg) : io;

	if	(n > 1)
	{
		XCATPAR *cat;

		n--;
		cat = memalloc(sizeof *cat + n * sizeof cat->tab[0]);
		cat->idef = idef;
		cat->path = mstrcpy(meta->path);
		cat->opt = mstrcpy(mode->opt);
		cat->arg = mstrcpy(mode->arg);
		cat->tab = (void *) (cat + 1);
		cat->dim = n;
		cat->par = par;
		meta->cur->read = xcat_read;
		meta->cur->ipar = rd_init(&xcat_reftype, cat);

		for (i = 0; i < n; i++)
			cat->tab[i] = mstrcpy(EDBMeta_next(meta));
	}
	else	edb_input(meta->cur, idef->in_read, par);

	memfree(mode);
}
