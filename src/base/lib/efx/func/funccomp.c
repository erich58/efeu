/*	Sortierfunktion für Funktionen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4

Die Sortierordnung bestimmt die Funktionssuche.

Funktionen mit fixer Argumentzahl kommen vor Funktionen mit variabler
Argumentzahl und werden mit aufsteigender Argumentzahl sortiert.
Funktionen mit variabler Argumentzahl werden nach absteigender
Argumentzahl sortiert.
Bei gleichem Datentype werden L-Werte vor Konstanten gereiht.
Bei verschiedenen Datentype wird das Funktionsgewicht berücksichtigt.
*/

#include <EFEU/object.h>


int FuncComp (const void *pa, const void *pb)
{
	int i;
	Func_t *a, *b;

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
			/*
			if (a->arg[i].type == NULL)	return 1;
			if (b->arg[i].type == NULL)	return -1;
			*/
			if (a->weight < b->weight)	return -1;
			if (a->weight > b->weight)	return 1;

			if (a->arg[i].type < b->arg[i].type) return 1;

			return -1;
		}
	}

	return 0;
}
