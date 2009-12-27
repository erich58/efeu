/*
Ausgabefilter für term

$Copyright (C) 1999 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <term.h>
#include <efeudoc.h>
#include <ctype.h>


void term_att (Term *trm, int flag, char *att)
{
	if	(flag)		pushstack(&trm->s_att, trm->att);
	else			att = popstack(&trm->s_att, NULL);

	if	(att == trm->att)	return;

	if	(trm->att)	io_puts(term_par.rm, trm->out);
	if	(att)		io_puts(att, trm->out);

	trm->att = att;
}


void term_newline (Term *trm, int flag)
{
	if	(trm->col)
		io_putc('\n', trm->out);

	if	(trm->hang)
	{
		trm->var.margin += trm->hang;
		trm->hang = 0;
	}

	trm->col = 0;
	trm->mode = 0;
	trm->space = 0;

	if	(flag)
		io_putc('\n', trm->out);
}

void term_hmode (Term *trm)
{
	if	(trm->col < trm->var.margin)
		trm->col += io_nputc(' ', trm->out, trm->var.margin - trm->col);
	else if	(trm->space)
		trm->col += io_nputc(' ', trm->out, 1);

	trm->space = 0;
	trm->mode = 1;
}

int term_putc (void *drv, int c)
{
	Term *trm = drv;

	if	(c == 0)
	{
		term_newline(trm, 0);
	}
	else if	(isspace(c))
	{
		if	(trm->col > term_par.wpmargin)
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

int term_verb (void *drv, int c)
{
	Term *trm = drv;

	if	(c != '\n')
	{
		term_hmode(trm);
		io_putc(c, trm->out);
		trm->col++;
	}
	else	term_newline(trm, trm->col == 0);

	return c;
}

void term_string (Term *trm, const char *str)
{
	if	(str == NULL)	return;

	for (; *str != 0; str++)
		trm->put(trm, *str);
}

