#include "CubeHandle.h"

static STRBUF(lbuf, 1000);

void label_beg (void)
{
	lbuf.pos = 0;
	sb_sync(&lbuf);
}

size_t label_pos (void)
{
	return lbuf.pos;
}

void label_add (size_t pos, mdaxis *x, size_t n)
{
	lbuf.pos = pos;
	sb_sync(&lbuf);

	if	(pos)
		sb_putc('.', &lbuf);

	sb_puts(StrPool_get(x->sbuf, x->idx[n].i_name), &lbuf);
}

void label_xadd (size_t pos, mdaxis *x, size_t n)
{
	lbuf.pos = pos;
	sb_sync(&lbuf);
	sb_putc('"', &lbuf);
	utf8_save(StrPool_get(x->sbuf, x->idx[n].i_name), &lbuf);
	sb_putc('"', &lbuf);
	sb_putc(';', &lbuf);
}

char *label_get (void)
{
	sb_putc(0, &lbuf);
	return (char *) lbuf.data;
}
