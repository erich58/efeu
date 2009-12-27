/*
Daten zählen

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>

static void subadd (mdaxis *x, char *ptr, MdCntObj *cnt);


void md_count (mdmat *ctab, const void *data)
{
	register MdCntGrp *s;
	register mdaxis *x;
	MdCntObj *cnt;

/*	Zähler setzen
*/
	cnt = ctab->priv;

	if	(cnt->set && cnt->set(data) == 0)
		return;

/*	Selektionsindizes setzen
*/
	for (x = ctab->axis; x && x->priv; x = x->next)
	{
		for (s = x->priv; s != NULL; s = s->next)
		{
			s->idx = MdClassify(s->cdef, data);
			s->flag = (s->idx >= 0 && s->idx < s->cdef->dim);
		}
	}

/*	Addition durchführen
*/
	subadd(ctab->axis, (char *) ctab->data, cnt);
}


/*	Additionshilfsprogramm
*/

static void subadd (mdaxis *x, char *ptr, MdCntObj *cnt)
{
	if	(x && x->priv)
	{
		MdCntGrp *s;

		for (s = x->priv; s != NULL; s = s->next)
		{
			if	(s->flag)
			{
				subadd(x->next, ptr + s->idx * x->size, cnt);
			}
	
			ptr += s->cdef->dim * x->size;
		}
	}
	else	cnt->add(ptr);
}
