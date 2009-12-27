/*
Ausgabe in Datei
*/

#include <EFEU/EDB.h>

#define	NAME_FMT	"%s.%02d%s"

typedef struct {
	REFVAR;
	char *lastname;
	char *name;
	char *ext;
	EDBPrintMode pmode;
	size_t count;
	size_t idx;
	size_t (*write) (EfiType *type, void *data, void *par);
	void *opar;
	IO *base;
	EDB *edb;
} XOUT;

static void xout_clean (void *data)
{
	XOUT *xout = data;
	rd_debug(xout, "%s: %u byte", xout->lastname, xout->count);
	rd_deref(xout->opar);
	memfree(xout->lastname);
	memfree(xout->name);
	io_close(xout->base);
	memfree(xout);
}

static RefType xout_reftype = REFTYPE_INIT("EDB_XOUT", NULL, xout_clean);

static void make_base (XOUT *xout)
{
	char *p, *name;
	
	name = msprintf(NAME_FMT, xout->name, ++xout->idx, xout->ext);
	rd_debug(xout, "rename %s to %s\n", xout->lastname, name);

	if	(rename(xout->lastname, name) != 0)
	{
		perror(name);
		exit(EXIT_FAILURE);
	}

	xout->base = io_fileopen(xout->lastname, "wz");
	io_xprintf(xout->base, "EDB\t1.0\n");

	if	(xout->edb->desc)
	{
		io_puts("// ", xout->base);

		for (p = xout->edb->desc; *p != 0; p++)
		{
			if	(*p != '\n')	io_putc(*p, xout->base);
			else if	(p[1])		io_puts("\n// ", xout->base);
			else			break;
		}

		io_putc('\n', xout->base);
	}

	p = strrchr(name, '/');
	io_xprintf(xout->base, "@file\n%s\n", p ? p + 1 : name);
	memfree(xout->lastname);
	xout->lastname = name;
}

static void xout_sub (EDB *edb, XOUT *xout);


static void xout_next (XOUT *xout)
{
	char *name;
	IO *io;

	name = msprintf(NAME_FMT, xout->name, ++xout->idx, xout->ext);
	io = io_fileopen(name, "wz");

	if	(xout->base)
	{
		char *p = strrchr(name, '/');
		io_xprintf(xout->base, "%s\n", p ? p + 1 : name);
	}

	memfree(xout->lastname);
	xout->lastname = name;
	edb_setout(xout->edb, io, &xout->pmode);
	xout_sub(xout->edb, xout);
}

static size_t write_xout (EfiType *type, void *data, void *par)
{
	XOUT *xout;
	size_t n;
	
	xout = par;

	if	(xout->count > xout->pmode.split)
	{
		rd_deref(xout->opar);
		xout->opar = NULL;

		if	(!xout->base)
			make_base(xout);

		rd_debug(xout, "%s: %u byte", xout->lastname, xout->count);
		xout_next(xout);
	}

	n = xout->write(type, data, xout->opar);
	xout->count += n;
	return n;
}

static void xout_sub (EDB *edb, XOUT *xout)
{
	xout->write = edb->write;
	xout->opar = edb->opar;
	xout->count = 0;
	edb->write = write_xout;
	edb->opar = xout;
}


static void xout (EDB *edb, const char *name, EDBPrintMode *pmode)
{
	XOUT *xout;
	char *p;
	
	xout = memalloc(sizeof *xout);
	xout->pmode = *pmode;
	xout->lastname = mstrcpy(name);

	p = strrchr(name, '.');

	if	(p && p != name && p[1] && !strchr(p - 1, '/'))
	{
		xout->name = mstrncpy(name, p - name);
		xout->ext = mstrcpy(p);
	}
	else
	{
		xout->name = mstrcpy(name);
		xout->ext = NULL;
	}

	xout->base = NULL;
	xout->edb = edb;
	rd_init(&xout_reftype, xout);
	rd_debug(xout, "%s: split limit %u byte",
		xout->name, xout->pmode.split);
	xout_sub(edb, xout);
}

void edb_fout (EDB *edb, const char *name, const char *mode)
{
	EDBPrintMode *pmode;
	IO *io;

	if	(!edb)	return;

	io = io_fileopen(name, "wz");
	pmode = edb_pmode(NULL, edb, mode);
	edb_setout(edb, io, pmode);

	if	(pmode->split && name && *name != '|' && *name != '&')
		xout(edb, name, pmode);
}
