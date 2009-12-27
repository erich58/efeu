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
#include <EFEU/Debug.h>

#define	DEBUG_FLAG	1

/*	Die folgenden Fehlermeldungen weisen auf einen Programmfehler hin
*/

#define	ERR_MSG1	"%s: fatal error: use of a deleted object (%p)\n"
#define	ERR_MSG2	"%s: fatal error: unkown size of object (%p)\n"

extern char *ProgIdent;

#if	DEBUG_FLAG

static int odebug_depth = 0;
static int odebug_flag = 0;
static int odebug_sync = 0;
static IO *odebug_log = NULL;

static void do_debug (int flag, char *fmt, const EfiObj *obj);
#else
#define do_debug(flag, fmt, obj)
#endif


/*	Zuweisungsobjekt generieren
*/

EfiObj *LvalObj (EfiLval *lval, EfiType *type, ...)
{
	EfiObj *x;
	va_list args;

	if	(!lval)
		lval = &Lval_data;

	va_start(args, type);
	x = lval->alloc(type, args);
	va_end(args);

	x->type = type;
	x->lval = lval;
	lval->update(x);
	do_debug(0, "new ", x);
	return x;
}

void UpdateLval (EfiObj *obj)
{
	if	(obj && obj->lval && obj->lval->update)
		obj->lval->update(obj);
}

void SyncLval (EfiObj *obj)
{
	if	(obj && obj->lval && obj->lval->sync)
		obj->lval->sync(obj);
}

void UpdateObjList (EfiObjList *list)
{
	for (; list != NULL; list = list->next)
		UpdateLval(list->obj);
}

void SyncObjList (EfiObjList *list)
{
	for (; list != NULL; list = list->next)
		SyncLval(list->obj);
}

/*	Neues Objekt generieren
*/

static EfiObj *newobj (const EfiType *type, void *data, const void *defval)
{
	EfiObj *obj;
	size_t size;

	if	(type == NULL)	return NULL;

	size = type->size + sizeof(EfiObj);
	obj = Obj_alloc(size);
	obj->lval = NULL;
	obj->type = (EfiType *) type;
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

EfiObj *NewObj (const EfiType *type, void *data)
{
	return newobj(type, data, NULL);
}

EfiObj *ConstObj (const EfiType *type, const void *data)
{
	return newobj(type, NULL, data);
}

EfiObj *NewPtrObj (const EfiType *type, const void *data)
{
	return newobj(type, &data, NULL);
}

#if	DEBUG_FLAG

static void do_debug (int flag, char *fmt, const EfiObj *obj)
{
	int i;

	if	(odebug_sync < DebugChangeCount)
	{
		odebug_sync = DebugChangeCount;
		io_puts("STOP Object debugging\n", odebug_log);
		rd_deref(odebug_log);
		odebug_log = rd_refer(LogOut("Obj", DBG_DEBUG));
		io_puts("START Object debugging\n", odebug_log);
	}

	if	(!odebug_log)	return;

	if	(flag == 2)
	{
		if	(!odebug_flag)
		{
			odebug_depth--;
			io_nputc('\t', odebug_log, odebug_depth);
			io_puts("}", odebug_log);
		}
	}
	else if	(odebug_flag)
	{
		io_puts(" {\n", odebug_log);
		odebug_depth++;
	}

	if	(flag != 2)
	{
		io_nputc('\t', odebug_log, odebug_depth);
		io_printf(odebug_log, fmt, obj->refcount);
		io_puts(obj->type ? obj->type->name : "NULL", odebug_log);
	}

	if	(flag != 1)
	{
		io_printf(odebug_log, " (%p)", obj);

		if	(obj->type && obj->data)
		{
			io_puts(" = 0x", odebug_log);

			for (i = 0; i < obj->type->size; i++)
				io_printf(odebug_log, "%02x",
					((unsigned char *) obj->data)[i]);

			if	(obj->type == &Type_str)
				io_printf(odebug_log, "(%#s)",
					Val_str(obj->data));
		}

		io_putc('\n', odebug_log);
		odebug_flag = 0;
	}
	else	odebug_flag = 1;
}

#endif

