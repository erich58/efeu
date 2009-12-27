/*
Verwalten von Argumentlisten

$Copyright (C) 2001 Erich Frühstück
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

#include <EFEU/ArgList.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>
#include <EFEU/io.h>

#define	BLKSIZE	32

static char *argl_ident (const void *data)
{
	const ArgList *par = data;

	if	(par->dim)
	{
		StrBuf *buf = sb_create(0);
		IO *io = io_strbuf(buf);
		int i;

		io_puts(par->data[0], io);

		for (i = 1; i < par->dim; i++)
			io_printf(io, " %#s", par->data[i]);

		io_close(io);
		return sb2str(buf);
	}

	return	mstrcpy("NULL");
}

static void argl_clean (void *data)
{
	ArgList *argl;
	int i;

	argl = data;

	for (i = 0; i < argl->dim; i++)
		memfree(argl->data[i]);

	argl->dim = 0;

	lfree(argl->data);
	memfree(argl);
}

/*
Die Datenstruktur |$1| definiert den Referenztype für die
Struktur |ArgList|.
*/

RefType ArgList_reftype = REFTYPE_INIT("ArgList", argl_ident, argl_clean);

/*
Die Funktion |$1| generiert eine neue Argumentliste.
*/

extern ArgList *arg_create (void)
{
	return rd_init(&ArgList_reftype, memalloc(sizeof(ArgList)));
}

/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den dynamisch generierten String <arg>.
*/

void arg_madd (ArgList *list, char *arg)
{
	if	(list)
	{
		if	(list->dim >= list->size)
		{
			char **save;
			size_t i;
			
			save = list->data;
			list->size += BLKSIZE;
			list->data = lmalloc(list->size * sizeof(char *));

			for (i = 0; i < list->dim; i++)
				list->data[i] = save[i];

			lfree(save);
		}

		list->data[list->dim++] = arg;
	}
	else	memfree(arg);
}

/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den konstanten String <arg>.
*/

void arg_cadd (ArgList *list, const char *arg)
{
	if	(list)	arg_madd(list, mstrcpy(arg));
}

/*
Die Funktion |$1| fragt den  <n>-ten Eintrag der Argumentliste <list> ab.
Falls <list> nicht definiert ist, oder außerhalb des zulässigen Bereichs
liegt, liefert |$1| einen Nullpointer.
Negative Werte für <n> werden vom Ende der Liste weg gerechnet, wobei
hier der Eintrag 0 nicht abgefragt wird!
*/

char *arg_get (ArgList *list, int n)
{
	if	(list == NULL)	return NULL;
	if	(n == 0)	return list->data[0];

	if	(n < 0)	n += list->dim;

	return (n > 0 && n < list->dim) ? list->data[n] : NULL;
}

/*
Die Funktion |$1| setzt den <n>-ten Eintrag der Argumentliste <list> auf
den dynamisch generierten String <val>. Bei Bedarf wird die Liste
vergrößert.
Negative Werte für <n> werden vom Ende der Liste weg gerechnet, wobei
hier der Eintrag 0 nicht gesetzt wird!
*/

extern void arg_set (ArgList *list, int n, char *val)
{
	if	(list && (n >= 0 || n + list->dim > 0))
	{
		if	(n < 0)	n += list->dim;

		if	(n + 1 >= list->size)
		{
			char **save;
			size_t i;
			
			save = list->data;
			list->size += BLKSIZE;
			list->data = lmalloc(list->size * sizeof(char *));

			for (i = 0; i < list->dim; i++)
				list->data[i] = save[i];

			for (i = list->dim; i < n; i++)
				list->data[i] = NULL;

			list->dim = n + 1;
			lfree(save);
		}
		else if	(n >= list->dim)
		{
			list->dim = n + 1;
		}
		else	memfree(list->data[n]);

		list->data[n] = val;
	}
	else	memfree(val);
}

/*
$SeeAlso
\mref{stddbg(3)},
\mref{arg_append(3)},
\mref{ArgList(7)}.
*/

