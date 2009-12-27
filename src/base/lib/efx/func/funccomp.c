/*
Sortierfunktion für Funktionen

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

#include <EFEU/object.h>

static int cmp_type (EfiType *a, EfiType *b)
{
	int r;

	if	(a == b)	return 0;
	if	(a == NULL)	return -1;
	if	(b == NULL)	return 1;

	if	((r = cmp_type(a->base, b->base)))
		return r;

	if	(a->dim < b->dim)	return -1;
	if	(a->dim > b->dim)	return 1;

	if	(a->order < b->order)	return -1;
	if	(a->order > b->order)	return 1;

	if	(a->size < b->size)	return -1;
	if	(a->size > b->size)	return 1;

	return strcmp(a->name, b->name);
}

/*
Die Sortierordnung bestimmt die Funktionssuche.
Funktionen mit fixer Argumentzahl kommen vor Funktionen mit variabler
Argumentzahl und werden mit aufsteigender Argumentzahl sortiert.
Funktionen mit variabler Argumentzahl werden nach absteigender
Argumentzahl sortiert.
Bei gleichem Datentype werden L-Werte vor Konstanten gereiht.
Bei verschiedenen Datentype wird das Funktionsgewicht berücksichtigt.

Datentypen werden folgend gereiht:
Kleine (size) Datentypen werden vorgereiht
*/


int FuncComp (const void *pa, const void *pb)
{
	int i;
	EfiFunc *a, *b;

	a = Val_func(pa);
	b = Val_func(pb);

/*	Vergleich der Argumentzahl
*/
	if	(a->vaarg)
	{
		if	(b->vaarg == 0)		return 1;
		if	(a->dim > b->dim)	return -1;
		if	(a->dim < b->dim)	return 1;
	}
	else if	(b->vaarg)
	{
		return -1;
	}
	else
	{
		if	(a->dim < b->dim)	return -1;
		if	(a->dim > b->dim)	return 1;
	}

/*	Gleiche Argumentzahl
*/
	for (i = 0; i < a->dim; i++)
	{
		if	(a->arg[i].lval != b->arg[i].lval)
		{
			return (a->arg[i].lval ? -1 : 1);
		}
		else if	(a->arg[i].nokonv != b->arg[i].nokonv)
		{
			return (a->arg[i].nokonv ? 1 : -1);
		}
		else if	(a->arg[i].type != b->arg[i].type)
		{
			if (a->weight < b->weight)	return -1;
			if (a->weight > b->weight)	return 1;

			return cmp_type(a->arg[i].type, b->arg[i].type);
		}
	}

	return cmp_type(a->type, b->type);
}
