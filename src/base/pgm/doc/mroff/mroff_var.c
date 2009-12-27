/*
Umgebungsvariablen

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

#include <mroff.h>
#include <efeudoc.h>

static ALLOCTAB(stack_tab, 32, sizeof(mroff_var_t));

static void copy_var(mroff_var_t *tg, mroff_var_t *src)
{
	tg->next = src->next;
	tg->caption = src->caption;
}

extern void mroff_push (mroff_t *mr)
{
	mroff_var_t *par = new_data(&stack_tab);
	copy_var(par, &mr->var);
	mr->var.next = par;
}

extern void mroff_pop (mroff_t *mr)
{
	if	(mr->var.next)
	{
		mroff_var_t *par = mr->var.next;
		copy_var(&mr->var, par);
		del_data(&stack_tab, par);
	}
}
