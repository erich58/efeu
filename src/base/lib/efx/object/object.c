/*	Objekte verwalten
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/refdata.h>

#define	MEMCHECK	0

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


#define	SIZE_PTR	(sizeof(Obj_t))
#define	SIZE_SMALL	(sizeof(Obj_t) + sizeof(long))
#define	SIZE_LARGE	(sizeof(Obj_t) + 4 * sizeof(void *))


#if	MEMCHECK
#define	CHECK_SIZE	8
#define	CHECK_MASK	"12345678"

static void mem_check(const Obj_t *obj);
#else
#define	CHECK_SIZE	0
#define	CHECK_MASK	""

#define	mem_check(obj)
#endif

static ALLOCTAB(tab_ptr, 0, SIZE_PTR);
static ALLOCTAB(tab_small, 0, SIZE_SMALL + CHECK_SIZE);
static ALLOCTAB(tab_large, 0, SIZE_LARGE + CHECK_SIZE);


/*	Dummyreferenzobjekt für LVAL-Werte
*/

REFTYPE(lval_reftype, "LVAL", NULL, NULL);

static refdata_t lval_refdata = { REFDATA(&lval_reftype) };


/*	Pointerobjekt generieren
*/

Obj_t *LvalObj(Type_t *type, void *lref, void *data)
{
	Obj_t *x;

	x = new_data(&tab_ptr);
	x->reftype = NULL;
	x->refcount = 1;
	x->type = type;
	x->data = data;
	x->lref = rd_refer(lref ? lref : &lval_refdata);
	do_debug(0, "new ", x);
	return x;
}

/*	Neues Objekt generieren
*/

static Obj_t *newobj(Type_t *type, void *data, const void *defval)
{
	Obj_t *obj;
	size_t size;

	if	(type == NULL)	return NULL;

	size = type->size + sizeof(Obj_t);

	if	(size <= SIZE_SMALL)	obj = new_data(&tab_small);
	else if	(size <= SIZE_LARGE)	obj = new_data(&tab_large);
	else				obj = lmalloc(size);

	obj->reftype = NULL;
	obj->refcount = 1;
	obj->lref = NULL;
	obj->type = type;
	obj->data = (obj + 1);

#if	MEMCHECK
	memcpy(((char *) obj) + size, CHECK_MASK, CHECK_SIZE);
#endif

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
	mem_check(obj);

	if	(obj)
	{
		do_debug(0, "ref[%d] ", obj);

		if	(obj->refcount == 0)
		{
			fprintf(stderr, ERR_MSG1, ProgIdent, (ulong_t) obj);
			libexit(EXIT_FAILURE);
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
		libexit(EXIT_FAILURE);
	}

	do_debug(1, "delete ", obj);

	if	(obj->lref == NULL && cleanup)
		CleanData(obj->type, obj->data);

	obj->refcount = 0;
	do_debug(2, NULL, obj);

	if	(obj->lref)
	{
		rd_deref(obj->lref);
		del_data(&tab_ptr, obj);
	}
	else if	(obj->type)
	{
		size_t size = sizeof(Obj_t) + obj->type->size;

		if	(size <= SIZE_SMALL)	del_data(&tab_small, obj);
		else if	(size <= SIZE_LARGE)	del_data(&tab_large, obj);
		else				lfree(obj);
	}
	else
	{
		fprintf(stderr, ERR_MSG2, ProgIdent, (ulong_t) obj);
		libexit(EXIT_FAILURE);
	}
}


void UnrefObj(Obj_t *obj)
{
	mem_check(obj);

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
#if	0
	else if	(obj->lref)
	{
		do_debug(1, "delete ", obj);
		obj->refcount = 0;
		del_data(&tab_ptr, obj);
		do_debug(2, NULL, obj);
	}
	else if	(obj->type)
	{
		size_t size;

		do_debug(1, "delete ", obj);
		CleanData(obj->type, obj->data);
		obj->refcount = 0;
		do_debug(2, NULL, obj);
		size = sizeof(Obj_t) + obj->type->size;

		if	(size <= SIZE_SMALL)	del_data(&tab_small, obj);
		else if	(size <= SIZE_LARGE)	del_data(&tab_large, obj);
		else				lfree(obj);
	}
#endif
}

/*	Objekt löschen
*/

void DeleteObj(Obj_t *obj)
{
	mem_check(obj);

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

#if	MEMCHECK

#define	CHECK_MSG	"%s: Speicherfehler von Objekt %lx, Type %#s.\n"

void mem_check(const Obj_t *obj)
{
	size_t size;
	
	if	(obj == NULL || obj->lref)	return;

	size = sizeof(Obj_t) + obj->type->size;

	if	(memcmp(((const char *) obj) + size, CHECK_MASK, CHECK_SIZE) != 0)
		io_printf(ioerr, CHECK_MSG, ProgIdent, (ulong_t) obj, obj->type->name);
}

#endif
