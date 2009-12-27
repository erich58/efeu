/*
Datenwerte formatieren

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
#include <EFEU/printobj.h>
#include <EFEU/fmtkey.h>

/*	Abfragefunktionen
*/

static EfiObj *get_arg (EfiObjList **ptr)
{
	if	(*ptr != NULL)
	{
		register EfiObj *x = RefObj((*ptr)->obj);
		*ptr = (*ptr)->next;
		return x;
	}
	else	return NULL;
}


int PrintFmtObj (IO *io, const char *fmt, const EfiObj *obj)
{
	EfiObjList *list;
	int n;

	list = NewObjList(RefObj(obj));
	n = PrintFmtList(io, fmt, list);
	DelObjList(list);
	return n;
}


/*	Formatierungsfunktion
*/

int PrintFmtList (IO *io, const char *fmt, EfiObjList *list)
{
	FmtKey key;
	EfiObj *obj;
	long lval;
	int n;

	if	(fmt == NULL)	return 0;

	n = 0;

	while (*fmt != 0)
	{
		if	(*fmt != '%')
		{
			if	(io_putc(*fmt, io) != EOF)	n++;

			fmt++;
			continue;
		}

		fmt++;
		fmt += fmtkey(fmt, &key);

		if	(key.flags & FMT_NEED_WIDTH)
		{
			key.width = Obj2long(get_arg(&list));

			if	(key.width < 0)
			{
				key.flags ^= FMT_RIGHT;
				key.width = -key.width;
			}
		}

		if	(key.flags & FMT_NEED_PREC)
		{
			key.prec = Obj2long(get_arg(&list));

			if	(key.prec < 0)
			{
				key.flags ^= FMT_NEGPREC;
				key.prec = -key.prec;
			}
		}

		switch (key.mode)
		{
		case 's':
		case 'S':
		
			if	((obj = EvalObj(get_arg(&list), &Type_str)))
			{
				n += fmt_str(io, &key, Val_str(obj->data));
				UnrefObj(obj);
			}

			break;

		case 'c':
		case 'C':

			n += fmt_char(io, &key, Obj2char(get_arg(&list)));
			break;

		case 'i':
		case 'd':
		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			obj = get_arg(&list);

			if	(key.flags & FMT_LONG)
				lval = Obj2long(obj);
			else if	(key.flags & FMT_SHORT)
				lval = Obj2short(obj);
			else	lval = Obj2int(obj);

			n += fmt_long(io, &key, lval);
			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':

			obj = get_arg(&list);
			n += fmt_double(io, &key, Obj2double(obj));
			break;

		case 'p':

			obj = get_arg(&list);
			n += fmt_long(io, &key, (obj ? (long) obj : 0));
			memfree(obj);
			break;

		case 'n':

			obj = get_arg(&list);

			if	(obj && obj->lval)
			{
				EfiObj *x;

				if	((x = EvalObj(long2Obj(n), obj->type)))
				{
					CleanData(obj->type, obj->data);
					CopyData(obj->type, obj->data, x->data);
					UnrefObj(x);
				}
			}

			UnrefObj(obj);
			break;

		case '[':

			n += fmt_str(io, &key, key.list);
			memfree(key.list);
			break;

		default:

			if	(io_putc(key.mode, io) != EOF)	n++;

			break;
		}
	}

	return n;
}
