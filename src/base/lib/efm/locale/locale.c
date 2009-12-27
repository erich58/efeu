/*
Sprachanpassung

$Copyright (C) 1997 Erich Frühstück
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

#include <EFEU/locale.h>
#include <EFEU/memalloc.h>
#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

/*
:*:the table |$1| contains locale for value representation
:de:Der Vektor |$1| enthält Locale zur Wertedarstellung
*/

LCValueDef LCValue[] = {
	{ "C", NULL, ".", "-", "+", " " },
	{ "de", ".", ",", "-", "+", " " },
	{ "TeX", ".", ",", "$-$", "$+$", " " },
	{ "doc", ".", ",", "\\minus ", "\\plus ", " " },
};

/*
static vectors for month names and weekday names
*/

static char *M_en[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December",
};

static char *W_en[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};

static char *M_de[12] = {
	"Jannuar", "Februar", "März", "April", "Mai", "Juni", "Juli",
	"August", "September", "Oktober", "November", "Dezember"
};

static char *M_de_AT[12] = {
	"Jänner", "Februar", "März", "April", "Mai", "Juni", "Juli",
	"August", "September", "Oktober", "November", "Dezember"
};

static char *W_de[7] =	{
	"Sonntag", "Montag", "Dienstag", "Mittwoch",
	"Donnerstag", "Freitag", "Samstag"
};

/*
:*:the table |$1| contains locale for data representation
:de:Der Vektor |$1| enthält Locale zur Datumsdarstellung
*/

LCDateDef LCDate[] = {
	{ "C", M_en, W_en },
	{ "de_AT", M_de_AT, W_de },
	{ "de", M_de, W_de },
};

LocaleDef Locale = { LCValue, LCValue, LCDate };

/*	Lokale Stack
*/

struct LCStack {
	struct LCStack *next;	/* Lokalestack */
	LocaleDef locale;	/* Lokaledaten */
};

static ALLOCTAB(LCTAB, "LCStack", 8, sizeof(struct LCStack));

static struct LCStack *LocaleStack = NULL;


void PushLocale(void)
{
	struct LCStack *x = LocaleStack;
	LocaleStack = new_data(&LCTAB);
	LocaleStack->next = x;
	LocaleStack->locale = Locale;
}


void PopLocale(void)
{
	if	(LocaleStack)
	{
		struct LCStack *x = LocaleStack;
		LocaleStack = x->next;
		Locale = x->locale;
		del_data(&LCTAB, x);
	}
}


/*	Lokale suchen
*/

static int lc_comp (const char *name, const char *key)
{
	while (*name)
	{
		if	(*key != *name)	return 0;

		name++;
		key++;
	}

	return 1;
}

static void *lc_search (void *tab, size_t dim, size_t elsize, const char *key)
{
	struct { char *name; } *ptr;
	int i;

	if	(key == NULL)	return tab;

	for (i = 0; i < dim; i++)
	{
		ptr = (void *) ((char *) tab + i * elsize);

		if	(ptr->name && lc_comp(ptr->name, key))
			return ptr;
	}

	return tab;
}

void SetLocale (unsigned type, const char *name)
{
	if	(type & LOC_PRINT)
		Locale.print = lc_search(tabparm(LCValue), name);

	if	(type & LOC_SCAN)
		Locale.scan = (type & LOC_PRINT) ? Locale.print :
			lc_search(tabparm(LCValue), name);

	if	(type & LOC_DATE)
		Locale.date = lc_search(tabparm(LCDate), name);
}
