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
		StrBuf *buf = sb_acquire();
		IO *io = io_strbuf(buf);
		int i;

		io_puts(StrPool_get(data, par->index[0]), io);

		for (i = 1; i < par->dim; i++)
			io_xprintf(io, " %#s", StrPool_get(data, par->index[i]));

		io_close(io);
		return sb_cpyrelease(buf);
	}

	return	mstrcpy("NULL");
}

static void argl_clean (void *data)
{
	ArgList *argl = data;
	StrPool_clean(data);
	lfree(argl->index);
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
	ArgList *arg = memalloc(sizeof *arg);
	StrPool_init((void *) arg);
	return rd_init(&ArgList_reftype, arg);
}

/*
Die Funktion |$1| beginnt ein neues Argument und liefert die
Argumentliste als StrPool-Pointer.
*/

StrPool *arg_next (ArgList *list)
{
	if	(!list)	return NULL;

	if	(list->dim >= list->size)
	{
		list->size += BLKSIZE;
		list->index = lrealloc(list->index,
			list->size * sizeof list->index[0]);
	}

	list->index[list->dim++] = StrPool_offset((StrPool *) list);
	return (StrPool *) list;
}

/*
Die Funktion |$1| entfernt den letzten Argumenteintrag.
*/

void arg_undo (ArgList *list)
{
	if	(list && list->dim)
	{
		list->dim--;

		if	(list->index[list->dim] > list->csize)
			list->used = list->index[list->dim] - list->csize;
	}
}

/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den konstanten String <arg>.
*/

void arg_cadd (ArgList *list, const char *arg)
{
	StrPool_xadd(arg_next(list), arg);
}

/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den dynamisch generierten String <arg>.
*/

void arg_madd (ArgList *list, char *arg)
{
	StrPool_xadd(arg_next(list), arg);
	memfree(arg);
}

/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den durch die Formatanweisung definierten String.
*/

void arg_printf (ArgList *list, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	StrPool_vprintf(arg_next(list), fmt, args);
	va_end(args);
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
	if	(list == NULL)
		return NULL;

	if	(n == 0)
		return StrPool_get((StrPool *) list, list->index[0]);

	if	(n < 0)
		n += list->dim;

	return (n > 0 && n < list->dim) ?
		StrPool_get((StrPool *) list, list->index[n]) : NULL;
}

int arg_test (ArgList *list, int n)
{
	return (list && n >= 0 && n < list->dim);
}

/*
Die Funktion |$1| setzt den <n>-ten Eintrag der Argumentliste <list> auf
<val>. Bei Bedarf wird die Liste vergrößert.
Negative Werte für <n> werden vom Ende der Liste weg gerechnet, wobei
hier der Eintrag 0 nicht gesetzt wird!
*/

extern void arg_cset (ArgList *list, int n, const char *val)
{
	if	(list && (n >= 0 || n + list->dim > 0))
	{
		if	(n < 0)	n += list->dim;

		if	(n + 1 >= list->size)
		{
			size_t i;

			list->size += BLKSIZE;
			list->index = lrealloc(list->index,
				list->size * sizeof list->index[0]);

			for (i = list->dim; i < n; i++)
				list->index[i] = 0;

			list->dim = n + 1;
		}
		else if	(n >= list->dim)
		{
			list->dim = n + 1;
		}

		list->index[n] = StrPool_add((StrPool *) list, val);
	}
}

/*
Wie |arg_cset|, jedoch mit einem dynamisch generierten String <arg>.
*/

void arg_mset (ArgList *list, int n, char *val)
{
	arg_cset(list, n, val);
	memfree(val);
}


/*
$SeeAlso
\mref{stddbg(3)},
\mref{arg_append(3)},
\mref{ArgList(7)}.
*/

