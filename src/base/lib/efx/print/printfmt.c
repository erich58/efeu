/*	Datenwerte formatieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/fmtkey.h>

/*	Abfragefunktionen
*/

static Obj_t *getarg (ObjList_t **ptr)
{
	if	(*ptr != NULL)
	{
		register Obj_t *x = RefObj((*ptr)->obj);
		*ptr = (*ptr)->next;
		return x;
	}
	else	return NULL;
}


int PrintFmtObj (io_t *io, const char *fmt, const Obj_t *obj)
{
	ObjList_t *list;
	int n;

	list = NewObjList(RefObj(obj));
	n = PrintFmtList(io, fmt, list);
	DelObjList(list);
	return n;
}


/*	Formatierungsfunktion
*/

int PrintFmtList (io_t *io, const char *fmt, ObjList_t *list)
{
	fmtkey_t key;
	Obj_t *obj;
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
			key.width = Obj2long(getarg(&list));

			if	(key.width < 0)
			{
				key.flags ^= FMT_RIGHT;
				key.width = -key.width;
			}
		}

		if	(key.flags & FMT_NEED_PREC)
		{
			key.prec = Obj2long(getarg(&list));

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
		
			if	((obj = EvalObj(getarg(&list), &Type_str)))
			{
				n += fmt_str(io, &key, Val_str(obj->data));
				UnrefObj(obj);
			}

			break;

		case 'c':
		case 'C':

			n += fmt_char(io, &key, Obj2char(getarg(&list)));
			break;

		case 'i':
		case 'd':
		case 'u':
		case 'b':
		case 'o':
		case 'x':
		case 'X':

			n += fmt_long(io, &key, Obj2long(getarg(&list)));
			break;

		case 'f':
		case 'e':
		case 'E':
		case 'g':
		case 'G':

			n += fmt_double(io, &key, Obj2double(getarg(&list)));
			break;

		case 'p':

			obj = getarg(&list);
			n += fmt_long(io, &key, (obj ? (long) obj : 0));
			FREE(obj);
			break;

		case 'n':

			obj = getarg(&list);

			if	(obj && obj->lref)
			{
				Obj_t *x;

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
			FREE(key.list);
			break;

		default:

			if	(io_putc(key.mode, io) != EOF)	n++;

			break;
		}
	}

	return n;
}
