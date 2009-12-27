/*
Objekte verwalten

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
#include <EFEU/refdata.h>

#define	DEBUG_FLAG	1

/*	Die folgenden Fehlermeldungen weisen auf einen Programmfehler hin
*/

#define	ERR_MSG1	"%s: fatal error: use of a deleted object (%lx)\n"
#define	ERR_MSG2	"%s: fatal error: unkown size of object (%lx)\n"

extern char *ProgIdent;

#if	DEBUG_FLAG
int ObjDebugFlag = 0;
static int depth = 0;
static int delflag = 0;

static void do_debug (int flag, char *fmt, const Obj_t *obj);
#else
#define do_debug(flag, fmt, obj)
#endif


/*	Zuweisungsobjekt generieren
*/

Obj_t *LvalObj (Lval_t *lval, Type_t *type, ...)
{
	Obj_t *x;
	va_list args;

	va_start(args, type);
	x = lval->alloc(type, args);
	va_end(args);

	x->reftype = NULL;
	x->refcount = 1;
	x->type = type;
	x->lval = lval;
	lval->update(x);
	do_debug(0, "new ", x);
	return x;
}

void SyncLval (Obj_t *obj)
{
	if	(obj && obj->lval && obj->lval->sync)
		obj->lval->sync(obj);
}

/*	Neues Objekt generieren
*/

static Obj_t *newobj(Type_t *type, void *data, const void *defval)
{
	Obj_t *obj;
	size_t size;

	if	(type == NULL)	return NULL;

	size = type->size + sizeof(Obj_t);
	obj = Obj_alloc(size);
	obj->reftype = NULL;
	obj->refcount = 1;
	obj->lval = NULL;
	obj->type = type;
	obj->data = (obj + 1);

	if	(data)
	{
		memcpy(obj->data, data, type->size);
	}
	else if	(defval)
	{
		CopyData(type, obj->data, defval);
	}
	else if	(type->defval)
	{
		CopyData(type, obj->data, type->defval);
	}
	else	memset(obj->data, 0, type->size);

	do_debug(0, "new ", obj);
	return obj;
}

Obj_t *NewObj(Type_t *type, void *data)
{
	return newobj(type, data, NULL);
}

Obj_t *ConstObj(Type_t *type, const void *data)
{
	return newobj(type, NULL, data);
}

Obj_t *NewPtrObj(Type_t *type, const void *data)
{
	return newobj(type, &data, NULL);
}


/*	Referenzzähler erhöhen
*/

Obj_t *RefObj(const Obj_t *obj)
{
	Obj_check(obj);

	if	(obj)
	{
		do_debug(0, "ref[%d] ", obj);

		if	(obj->refcount == 0)
		{
			fprintf(stderr, ERR_MSG1, ProgIdent, (ulong_t) obj);
			exit(EXIT_FAILURE);
		}

		((Obj_t *) obj)->refcount++;
		return (Obj_t *) obj;
	}

	return NULL;
}


/*	Referenzzähler verringern/Objekt freigeben
*/

static void del_obj(Obj_t *obj, int cleanup)
{
	if	(obj->refcount == 0)
	{
		do_debug(0, "delete ", obj);
		fprintf(stderr, ERR_MSG1, ProgIdent, (ulong_t) obj);
		exit(EXIT_FAILURE);
	}

	do_debug(1, "delete ", obj);

	if	(obj->lval == NULL && cleanup)
		CleanData(obj->type, obj->data);

	obj->refcount = 0;
	do_debug(2, NULL, obj);

	if	(obj->lval)
	{
		obj->lval->free(obj);
	}
	else if	(obj->type)
	{
		Obj_free(obj, sizeof(Obj_t) + obj->type->size);
	}
	else
	{
		fprintf(stderr, ERR_MSG2, ProgIdent, (ulong_t) obj);
		exit(EXIT_FAILURE);
	}
}


void UnrefObj(Obj_t *obj)
{
	Obj_check(obj);

	if	(obj == NULL)
	{
		return;
	}
	else if	(obj->refcount > 1)
	{
		obj->refcount--;
		do_debug(0, "deref[%d] ", obj);
	}
	else	del_obj(obj, 1);
}


/*	Objekt löschen
*/

void DeleteObj(Obj_t *obj)
{
	Obj_check(obj);

	if	(obj == NULL)
	{
		return;
	}
	else if	(obj->refcount > 1)
	{
		obj->refcount--;
		do_debug(0, "deref[%d] ", obj);
	}
	else	del_obj(obj, 0);
}


#if	DEBUG_FLAG

static void do_debug(int flag, char *fmt, const Obj_t *obj)
{
	int i;

	if	(!ObjDebugFlag)	return;

	if	(flag == 2)
	{
		if	(!delflag)
		{
			depth--;
			io_nputc('\t', ioerr, depth);
			io_puts("}", ioerr);
		}
	}
	else if	(delflag)
	{
		io_puts(" {\n", ioerr);
		depth++;
	}

	if	(flag != 2)
	{
		io_nputc('\t', ioerr, depth);
		io_printf(ioerr, fmt, obj->refcount);
		io_puts(obj->type ? obj->type->name : "NULL", ioerr);
	}

	if	(flag != 1)
	{
		io_printf(ioerr, " (%lx)", (ulong_t) obj);

		if	(obj->type)
		{
			io_puts(" = 0x", ioerr);

			for (i = 0; i < obj->type->size; i++)
				io_printf(ioerr, "%02x", ((uchar_t *) obj->data)[i]);

			if	(obj->type == &Type_str)
			{
				io_printf(ioerr, "(%#s)", Val_str(obj->data));
			}
		}

		io_putc('\n', ioerr);
		delflag = 0;
	}
	else	delflag = 1;
}

#endif

