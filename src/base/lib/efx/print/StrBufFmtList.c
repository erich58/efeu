/*
Datenwerte formatieren

$Copyright (C) 1994, 2008 Erich Frühstück
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
		EfiObj *x = RefObj((*ptr)->obj);
		*ptr = (*ptr)->next;
		return x;
	}
	else	return NULL;
}


/*	Formatierungsfunktion
*/

int StrBufFmtList (StrBuf *sb, const char *fmt, EfiObjList *list)
{
	FmtKey *key;
	EfiObj *obj;
	intmax_t lval;
	int n;
	int pos;

	if	(!sb || !fmt)	return 0;

	n = 0;
	pos = sb->pos;

	while (*fmt != 0)
	{
		if	(*fmt != '%')
		{
			sb_putucs(fmtkey_pgetucs(&fmt), sb);
			n++;
			continue;
		}

		fmt++;
		key = fmtkey(NULL, fmtkey_pgetucs, &fmt);

		if	(key->flags & FMT_NEED_WIDTH)
		{
			key->width = Obj2int(get_arg(&list));

			if	(key->width < 0)
			{
				key->flags ^= FMT_RIGHT;
				key->width = -key->width;
			}
		}

		if	(key->flags & FMT_NEED_PREC)
		{
			key->prec = Obj2int(get_arg(&list));

			if	(key->prec < 0)
			{
				key->flags ^= FMT_NEGPREC;
				key->prec = -key->prec;
			}
		}

		switch (key->mode)
		{
		case EOF:
			break;
		case 's':
		case 'S':
		
			if	((obj = EvalObj(get_arg(&list), &Type_str)))
			{
				n += fmt_str(sb, key, Val_str(obj->data));
				UnrefObj(obj);
			}

			break;

		case 'c':
		case 'C':

			obj = get_arg(&list);

			if	(key->size == FMTKEY_LONG)
			{
				int32_t z;
				Obj2Data(obj, &Type_wchar, &z);
				n += fmt_ucs(sb, key, z);
			}
			else
			{
				unsigned char c;
				Obj2Data(obj, &Type_char, &c);
				n += fmt_char(sb, key, c);
			}

			break;

		case 'i':
		case 'd':
		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			obj = get_arg(&list);

			if	(key->size == FMTKEY_IMAX
				|| key->size == FMTKEY_XLONG)
			{
				Obj2Data(obj, &Type_int64, &lval);
			}
			else if	(key->size == FMTKEY_PDIFF)
			{
				int64_t z;
				Obj2Data(obj, &Type_int64, &z);
				lval = (ptrdiff_t) z;
			}
			else if	(key->size == FMTKEY_SIZE)
			{
				uint64_t z;
				Obj2Data(obj, &Type_uint64, &z);
				lval = (size_t) z;
			}
			else if	(key->size == FMTKEY_LONG)
			{
				int32_t z;
				Obj2Data(obj, &Type_int32, &z);
				lval = z;
			}
			else if	(key->size == FMTKEY_BYTE)
			{
				int8_t z;
				Obj2Data(obj, &Type_int8, &z);
				lval = z;
			}
			else if	(key->size == FMTKEY_SHORT)
			{
				int16_t z;
				Obj2Data(obj, &Type_int16, &z);
				lval = z;
			}
			else	lval = Obj2int(obj);

			n += fmt_intmax(sb, key, lval);
			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
		case 'a':
		case 'A':

			obj = get_arg(&list);
			n += fmt_double(sb, key, Obj2double(obj));
			break;

		case 'p':

			obj = get_arg(&list);
			n += fmt_long(sb, key, (obj ? (long) obj : 0));
			memfree(obj);
			break;

		case 'n':

			obj = get_arg(&list);

			if	(obj && obj->lval)
			{
				EfiObj *x = int2Obj(key->flags & FMT_ALTMODE ?
					n : sb->pos - pos);

				if	((x = EvalObj(x, obj->type)))
				{
					CleanData(obj->type, obj->data, 0);
					CopyData(obj->type, obj->data, x->data);
					UnrefObj(x);
				}
			}

			UnrefObj(obj);
			break;

		case '[':

			n += fmt_str(sb, key, (char *) key->buf.data);
			break;

		default:

			if	(sb_putc(key->mode, sb) != EOF)	n++;

			break;
		}
	}

	return n;
}
