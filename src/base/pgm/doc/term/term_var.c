/*
Stack für Umgebungsvariablen

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

static ALLOCTAB(Stackab, "TermVar", 32, sizeof(TermVar));

static void TermVar_copy(TermVar *tg, TermVar *src)
{
	tg->next = src->next;
	tg->margin = src->margin;
	tg->caption = src->caption;
}

extern void term_push (Term *trm)
{
	TermVar *par = new_data(&Stackab);
	TermVar_copy(par, &trm->var);
	trm->var.next = par;
}

extern void term_pop (Term *trm)
{
	if	(trm->var.next)
	{
		TermVar *par = trm->var.next;
		TermVar_copy(&trm->var, par);
		del_data(&Stackab, par);
	}
}
