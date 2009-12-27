/*	Ausgabestruktur
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>

static ALLOCTAB(output_tab, 0, sizeof(EisDocOutput_t));

void EisDocOutput_push (EisDocOutput_t **output, io_t *io)
{
	EisDocOutput_t *x = new_data(&output_tab);

	if	(*output)
	{
		*x = **output;
		x->next = *output;
	}
	else	memset(x, 0, sizeof(EisDocOutput_t));

	if	(io)
	{
		x->io = io;
	}
	else	rd_refer(x->io);

	*output = x;
}

void EisDocOutput_pop (EisDocOutput_t **output)
{
	if	(*output)
	{
		EisDocOutput_t *x = *output;
		*output = x->next;
		rd_deref(x->io);
		del_data(&output_tab, x);
	}
}

void EisDocOutput_putc (EisDocOutput_t *output, int c)
{
	if	(output)	io_putc(c, output->io);
}

void EisDocOutput_puts (EisDocOutput_t *output, const char *s)
{
	if	(output)	io_puts(s, output->io);
}

void EisDocOutput_char (EisDocOutput_t *output, const char *s)
{
	if	(output == NULL)	return;

	if	(output->mode)
	{
		io_putc('(', output->io);
		io_xputs(s, output->io, ")");
		io_putc(')', output->io);
	}
	else	io_puts(s, output->io);
}

void EisDocOutput_charkey (EisDocOutput_t *output, int c)
{
	char buf[2];
	buf[0] = c;
	buf[1] = 0;
	EisDocOutput_char(output, buf);
}
