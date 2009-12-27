/*	Ausgabekontrolle für mdprint
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pctrl.h>
#include <EFEU/ioctrl.h>

static int pf_put (int c, PrFilter_t *pf);
static int pf_ctrl (PrFilter_t *pf, int req, va_list list);


/*	Druckkontrolle generieren
*/

io_t *io_PrCtrl(io_t *io, PrCtrl_t *pc)
{
	PrFilter_t *pf;

	if	(io == NULL || pc == NULL)	return io;

	pf = memalloc(sizeof(PrFilter_t));
	pf->pc = pc;
	pf->io = io;
	pf->line = 0;
	pf->col = 0;
	pf->page = 0;
	pf->delim = PrintListDelim;
	pf->nl = pc->nl;
	pf->nf = pc->nf;
	pf->put = NULL;
	pf->ctrl = pc->ctrl;

	io = io_alloc();
	io->put = (io_put_t) pf_put;
	io->ctrl = (io_ctrl_t) pf_ctrl;
	io->data = pf;
	io_ctrl(io, PCTRL_BEGIN);
	return io;
}


/*	Zeichen ausgeben
*/

static int pf_put(int c, PrFilter_t *pf)
{
	if	(pf->put)
	{
		return pf->put(c, pf);
	}

	/*
	return io_printf(pf->io, " %#c", c);
	*/
	return 0;
}


/*	Kontrollfunktion
*/

static int pf_ctrl(PrFilter_t *pf, int req, va_list list)
{
	if	(req == IO_CLOSE)
	{
		int stat;

		if	(pf->ctrl)
			pf->ctrl(pf, PCTRL_END, list);

		stat = io_close(pf->io);
		FREE(pf);
		return stat;
	}

	if	((req & ~0xff) != PCTRL_MASK)
		return io_vctrl(pf->io, req, list);

	if	(pf->put)
	{
		pf->put(EOF, pf);
		pf->put = NULL;
	}

	switch (req)
	{
	case PCTRL_YPOS:	return pf->line;
	case PCTRL_XPOS:	return pf->col;

	case PCTRL_BEGIN:
	
		pf->col = 0;
		pf->line = 1;
		pf->page = 1;
		break;

	case PCTRL_LINE:
	
		io_puts(pf->nl, pf->io);
		pf->col = 0;
		pf->line++;
		break;

	case PCTRL_PAGE:
	
		io_puts(pf->nf, pf->io);
		pf->line = 1;
		pf->page++;
		break;

	case PCTRL_LEFT:
	case PCTRL_CENTER:
	case PCTRL_RIGHT:
	case PCTRL_EMPTY:
	case PCTRL_VALUE:
	
		if	(pf->col++)	io_puts(pf->delim, pf->io);

		break;
	}

	if	(pf->ctrl)
		return pf->ctrl(pf, req, list);
	else	return EOF;
}
