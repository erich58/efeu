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
#include <EFEU/io.h>

#define	BLKSIZE	32

static char *argl_ident (ArgList_t *par)
{
	if	(par && par->dim)
	{
		strbuf_t *buf = new_strbuf(0);
		io_t *io = io_strbuf(buf);
		int i;

		io_puts(par->data[0], io);

		for (i = 1; i < par->dim; i++)
		{
			io_putc(' ', io);
			io_puts(par->data[i], io);
		}

		io_close(io);
		return sb2str(buf);
	}

	return	mstrcpy("NULL");
}

static ArgList_t *argl_admin (ArgList_t *tg, const ArgList_t *src)
{
	if	(tg)
	{
		int i;

		for (i = 0; i < tg->dim; i++)
			memfree(tg->data[i]);

		tg->dim = 0;

		if	(src == NULL)
		{
			lfree(tg->data);
			memfree(tg);
			tg = NULL;
		}
	}
	else	tg = src ? src : memalloc(sizeof(ArgList_t));

	return tg;
}

/*
Die Datenstruktur |$1| definiert den Referenztype für die
Struktur |ArgList_t|.
*/

ADMINREFTYPE(ArgList_reftype, "ArgList", argl_ident, argl_admin);

/*
Die Funktion |$1| generiert eine neue Argumentliste entsprechend der
Anweisungen in <def>.  Dabei handelt es sich um eine Zeichenkette aus
Kennbuchstaben mit der folgenden Bedeutung:
[n]	Null Pointer (kein Argument der Liste)
[c]	Konstante Zeichenkette
[m]	Dynamische Zeichenkette
[f]	Formatanweisung mit Argumenten
*/

ArgList_t *ArgList (const char *def, ...)
{
	ArgList_t *args;
	va_list list;

	args = rd_create(&ArgList_reftype);
	va_start(list, def);
	ArgList_append(args, def, list);
	va_end(list);
	return args;
}


/*
Die Funktion |$1| erweitert eine Argumentliste entsprechend der
Anweisungen in <def>.
*/

void ArgList_append (ArgList_t *args, const char *def, va_list list)
{
	char *p;

	if	(def == NULL)	return;

	for (; *def != 0; def++)
	{
		switch (*def)
		{
		case 'n':
			ArgList_madd(args, NULL);
			break;
		case 'c':
			ArgList_cadd(args, va_arg(list, char *));
			break;
		case 'm':
			ArgList_madd(args, va_arg(list, char *));
			break;
		case 'f':
			p = va_arg(list, char *);
			ArgList_madd(args, mvsprintf(p, list));
			break;
		default:
			break;
		}
	}
}


/*
Die Funktion |$1| erweitert die Argumentliste <list> um
den dynamisch generierten String <arg>.
*/

void ArgList_madd (ArgList_t *list, char *arg)
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

void ArgList_cadd (ArgList_t *list, const char *arg)
{
	if	(list)	ArgList_madd(list, mstrcpy(arg));
}

/*
Die Funktion |$1| fragt den  <n>-ten Eintrag der Argumentliste <list> ab.
Falls <list> nicht definiert ist, oder außerhalb des zulässigen Bereichs
liegt, liefert |$1| einen Nullpointer.
Negative Werte für <n> werden vom Ende der Liste weg gerechnet, wobei
hier der Eintrag 0 nicht abgefragt wird!
*/

char *ArgList_get (ArgList_t *list, int n)
{
	if	(list == NULL)	return NULL;
	if	(n == 0)	return list->data[0];

	if	(n < 0)	n += list->dim;

	return (n > 0 && n < list->dim) ? list->data[n] : NULL;
}

/*
$SeeAlso
\mref{Message(3)},
\mref{ArgList(7)}.
*/

