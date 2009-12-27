/*	Ausgabefilter für term
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <term.h>
#include <efeudoc.h>
#include <ctype.h>


void term_att (term_t *trm, int flag, char *att)
{
	if	(flag)		pushstack(&trm->s_att, trm->att);
	else			att = popstack(&trm->s_att, NULL);

	if	(att == trm->att)	return;

	if	(trm->att)	io_puts(TermPar.rm, trm->out);
	if	(att)		io_puts(att, trm->out);

	trm->att = att;
}


void term_newline (term_t *trm, int flag)
{
	if	(trm->col)
		io_putc('\n', trm->out);

	if	(trm->hang)
	{
		trm->margin += trm->hang;
		trm->hang = 0;
	}

	trm->col = 0;
	trm->mode = 0;
	trm->space = 0;

	if	(flag)
		io_putc('\n', trm->out);
}

void term_hmode (term_t *trm)
{
	if	(trm->col < trm->margin)
		trm->col += io_nputc(' ', trm->out, trm->margin - trm->col);
	else if	(trm->space)
		trm->col += io_nputc(' ', trm->out, 1);

	trm->space = 0;
	trm->mode = 1;
}

int term_putc (term_t *trm, int c)
{
	if	(c == 0)
	{
		term_newline(trm, 0);
	}
	else if	(isspace(c))
	{
		if	(trm->col > TermPar.wpmargin)
			term_newline(trm, 0);

		trm->space = trm->mode ? 1 : 0;
	}
	else
	{
		term_hmode(trm);
		io_putc(c, trm->out);
		trm->col++;
	}

	return c;
}

int term_verb (term_t *trm, int c)
{
	if	(c != '\n')
	{
		term_hmode(trm);
		io_putc(c, trm->out);
		trm->col++;
	}
	else	term_newline(trm, trm->col == 0);

	return c;
}

void term_string (term_t *trm, const char *str)
{
	if	(str == NULL)	return;

	for (; *str != 0; str++)
		trm->put((DocDrv_t *) trm, *str);
}

