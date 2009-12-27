/*	IO-Struktur mit Aufräumfunktion
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/ioctrl.h>


/*	Eingabestruktur
*/

typedef struct {
	void (*clean) (io_t *io, void *par);
	io_t *io;	/* Eingabestruktur */
	void *par;	/* Aufräumfunktion */
} CLPAR;


static int clpar_get (CLPAR *clpar)
{
	return io_getc(clpar->io);
}

static int clpar_put (int c, CLPAR *clpar)
{
	return io_putc(c, clpar->io);
}


static int clpar_ctrl (CLPAR *clpar, int req, va_list list)
{
	if	(req == IO_CLOSE)
	{
		int stat;

		clpar->clean(clpar->io, clpar->par);
		stat = io_close(clpar->io);
		memfree(clpar);
		return stat;
	}

	return io_vctrl (clpar->io, req, list);
}


io_t *io_cleanup (io_t *io, void (*cf) (io_t *io, void *p), void *p)
{
	if	(io && cf)
	{
		CLPAR *clpar = memalloc(sizeof(CLPAR));
		clpar->clean = cf;
		clpar->io = io;
		clpar->par = p;
		io = io_alloc();
		io->get = (io_get_t) clpar_get;
		io->put = (io_put_t) clpar_put;
		io->ctrl = (io_ctrl_t) clpar_ctrl;
		io->data = clpar;
	}

	return io;
}
