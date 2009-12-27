/*
Datenbank ausgeben

$Copyright (C) 1995 Erich Frühstück
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

#include <EFEU/database.h>
#include <EFEU/printobj.h>

#define	COLSEP	";"

/*
Indexwert abtesten
*/

struct IDXCMP {
	struct IDXCMP *next;	/* Für Verkettungen */
	char *pattern;		/* Musterkennung */
	int flag;		/* Negationsflag */
	size_t minval;		/* Minimalwert */
	size_t maxval;		/* Maximalwert */
	int (*cmp) (struct IDXCMP *t, const char *s, size_t n);
};

static struct IDXCMP *new_idxcmp (const char *def, size_t dim);
static void del_idxcmp (struct IDXCMP *ic);
static struct IDXCMP *idxcmplist (char **list, size_t ldim, size_t dim);
static int idxcmp (struct IDXCMP *ic, const char *name, size_t idx);


static ALLOCTAB(idxcmp_tab, 32, sizeof(struct IDXCMP));

static int pcmp (struct IDXCMP *t, const char *s, size_t n);
static int ncmp (struct IDXCMP *t, const char *s, size_t n);
static int xcmp (struct IDXCMP *t, const char *s, size_t n);


static int pcmp(struct IDXCMP *tst, const char *name, size_t idx)
{
	return patcmp(tst->pattern, name, NULL);
}


static int ncmp(struct IDXCMP *tst, const char *name, size_t idx)
{
	return (idx >= tst->minval && idx <= tst->maxval);
}


static int xcmp(struct IDXCMP *tst, const char *name, size_t idx)
{
	return 1;
}


static struct IDXCMP *new_idxcmp(const char *def, size_t dim)
{
	struct IDXCMP *x;;

	if	(def == NULL)	return NULL;

	x = new_data(&idxcmp_tab);
	memset(x, 0, sizeof(struct IDXCMP));
	x->flag = 1;
	x->cmp = xcmp;

	switch (*def)
	{	
		case '-':	x->flag = 0; def++; break;
		case '+':	def++; break;
		default:	break;
	}

	if	(*def == '#')
	{
		long a;
		char *p;

		a = strtol(def + 1, &p, 0);

		if	(-a > (long) dim)	a = 0;
		else if	(a < 0)			a += 1 + (long) dim;

		x->minval = a;

		if	(*p == ':')
		{
			a = strtol(p + 1, NULL, 0);
		}
		else	a = x->minval;

		if	(-a > (long) dim)	a = 0;
		else if	(a <= 0)		a += 1 + (long) dim;

		x->maxval = a;
		x->cmp = ncmp;
	}
	else if	(*def != 0)
	{
		x->pattern = mstrcpy(def);
		x->cmp = pcmp;
	}

	return x;
}


static struct IDXCMP *idxcmplist(char **list, size_t ldim, size_t dim)
{
	struct IDXCMP *test, **ptr;
	int i;

	test = NULL;
	ptr = &test;

	for (i = 0; i < ldim; i++)
	{
		*ptr = new_idxcmp(list[i], dim);
		ptr = &(*ptr)->next;
	}

	return test;
}


static int idxcmp(struct IDXCMP *test, const char *str, size_t idx)
{
	if	(test)
	{
		int flag;

		for (flag = !test->flag; test != NULL; test = test->next)
			if (test->cmp(test, str, idx)) flag = test->flag;

		return flag;
	}
	else	return 1;
}

static void del_idxcmp(struct IDXCMP *test)
{
	if	(test)
	{
		del_idxcmp(test->next);
		memfree(test->pattern);
		del_data(&idxcmp_tab, test);
	}
}

/*	Variablenliste zur Ausgabe generieren
*/

static size_t make_list (EfiStruct *st, char *list, EfiStruct ***ptr)
{
	size_t dim, idim;
	char **ilist;
	EfiStruct *x;
	struct IDXCMP *cmp;

	*ptr = NULL;

	if	(st == NULL)	return 0;

	for (x = st, dim = 0; x != NULL; x = x->next)
		dim++;

	idim = mstrsplit(list, "%s,;", &ilist);
	cmp = idxcmplist(ilist, idim, dim);
	memfree(ilist);
	*ptr = memalloc(dim * sizeof **ptr);
	dim = 0;

	for (x = st, dim = 0; x != NULL; x = x->next)
	{
		(*ptr)[dim] = idxcmp(cmp, x->name, dim) ? x : NULL;
		dim++;
	}

	del_idxcmp(cmp);
	return dim;
}


/*	Header ausgeben
*/

static void print_header (IO *io, EfiStruct **ptr, size_t dim)
{
	char *delim;
	int i;

	for (i = 0, delim = "#"; i < dim; i++)
	{
		if	(ptr[i])
		{
			io_puts(delim, io);
			delim = COLSEP;
			io_puts(ptr[i]->name, io);
		}
	}

	io_putc('\n', io);
}


/*	Daten ausgeben
*/

static void print_data (IO *io, int mode, char *data, EfiStruct **ptr, size_t dim)
{
	char *delim;
	int i;

	for (i = 0, delim = NULL; i < dim; i++)
	{
		if	(!ptr[i])	continue;

		io_puts(delim, io);

		if	(mode)
		{
			io_puts(ptr[i]->name, io);
			io_puts(" = ", io);
			delim = "\n";
		}
		else	delim = COLSEP;

		if	(ptr[i]->dim)
			PrintVecData(io, ptr[i]->type, data + ptr[i]->offset, ptr[i]->dim);
		else	PrintData(io, ptr[i]->type, data + ptr[i]->offset);
	}
}


/*	Datenbankwerte ausgeben
*/

void DB_save(IO *io, EfiDB *db, int mode, EfiVirFunc *test, char *list)
{
	size_t dim;
	EfiStruct **ptr;
	char *data;
	char *delim;
	EfiFunc *func;
	int testval;
	int i;

	if	(db == NULL)		return;

	dim = make_list(db->type->list, list, &ptr);
	func = test ? GetFunc(&Type_bool, test, 1, db->type, 0) : NULL;

	if	(mode)	;
	else if	(dim)	print_header(io, ptr, dim);
	else		io_puts("#this\n", io);

	data = db->buf.data;
	delim = NULL;

	for (i = 0; i < db->buf.used; i++)
	{
		testval = 1;

		if	(func)	CallFunc(&Type_bool, &testval, func, data);

		if	(testval)
		{
			io_puts(delim, io);
			delim = mode ? "\n" : NULL;

			if	(dim)	print_data(io, mode, data, ptr, dim);
			else		PrintData(io, db->type, data);

			io_putc('\n', io);
		}

		data += db->type->size;
	}

	memfree(ptr);
}
