/*	Ein/Ausgabefunktionen für MDMAT
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>

#define	IOFUNC(name, expr)	\
static void name (Func_t *func, void *rval, void **arg) \
{ register io_t *io = Val_io(arg[0]); Val_int(rval) = expr; }

#define	RV	Val_int(rval)


typedef size_t (*IoFunc_t) (io_t *io, char *data, size_t size);

typedef struct {
	char *name;
	int flag;
	IoFunc_t func;
} FUNCDEF;


static FUNCDEF iof_fread =  { "fread", 1, (IoFunc_t) get_MSBF };
static FUNCDEF iof_fwrite = { "fwrite", 0, (IoFunc_t) put_MSBF };
static FUNCDEF iof_rfread = { "rfread", 1, (IoFunc_t) get_LSBF };
static FUNCDEF iof_rfwrite = { "rfwrite", 0, (IoFunc_t) put_LSBF };

static int IoFunc_vec(io_t *io, FUNCDEF *f, const Type_t *type,
	char *data, size_t dim)
{
	Func_t *func;
	int i, n;

	n = 0;
	func = GetFunc(&Type_int, GetGlobalFunc(f->name), 2, &Type_io, 0, type, f->flag);

	if	(func != NULL)
	{
		void *arg[2];

		arg[0] = &io;

		for (i = 0; i < dim; i++)
		{
			int buf = 0;
			arg[1] = data;
			data += type->size;
			KonvRetVal(&Type_int, &buf, func, arg);
			n += buf;
		}
	}
	else
	{
		for (i = 0; i < dim; i++)
		{
			n += f->func(io, data, type->size);
			data += type->size;
		}
	}

	return n;
}


static int IoFunc_struct (io_t *io, FUNCDEF *f, Var_t *list, char *data)
{
	int n;

	for (n = 0; list != NULL; list = list->next)
		n += IoFunc_vec(io, f, list->type, data + list->offset,
			list->dim ? list->dim : 1);

	return n;
}

static int IoFunc_any (const char *name, void **arg, FUNCDEF *f)
{
	io_t *io;
	Obj_t *obj;
	int n;

	io = Val_io(arg[0]);
	obj = arg[1];

	if	(io == NULL || obj == NULL)
	{
		n = 0;
	}
	else if	(obj->type->dim)
	{
		n = IoFunc_vec(io, f, obj->type->base, obj->data, obj->type->dim);
	}
	else if	(obj->type->list)
	{
		n = IoFunc_struct(io, f, obj->type->list, obj->data);
	}
	else	n = f->func(io, obj->data, obj->type->size);

	return n;
}

static void fwrite_obj (Func_t *func, void *rval, void **arg)
{
	RV = IoFunc_any(func->name, arg, &iof_fwrite);
}

static void rfwrite_obj (Func_t *func, void *rval, void **arg)
{
	RV = IoFunc_any(func->name, arg, &iof_rfwrite);
}

static void fread_obj (Func_t *func, void *rval, void **arg)
{
	RV = IoFunc_any(func->name, arg, &iof_fread);
}

static void rfread_obj (Func_t *func, void *rval, void **arg)
{
	RV = IoFunc_any(func->name, arg, &iof_rfread);
}


static void dummy_IoFunc (Func_t *func, void *rval, void **arg)
{
	Val_int(rval) = 0;
}

static void xfread_bool (Func_t *func, void *rval, void **arg)
{
	Val_bool(arg[1]) = 0;
	RV = 1;

	switch (io_getc(Val_io(arg[0])))
	{
	case EOF:
		RV = 0; break;
	case '1':
	case 'T':
	case 't':
		Val_bool(arg[1]) = 1;
		break;
	default:
		break;
	}
}

static void xfwrite_bool (Func_t *func, void *rval, void **arg)
{
	RV = io_nputc(Val_bool(arg[1]) ? 't' : 'f', Val_io(arg[0]), 1);
}

#if	0
static void xfread_char (Func_t *func, void *rval, void **arg)
{
	int c = io_getc(Val_io(arg[0]));

	if	(c == EOF)
	{
		Val_char(arg[1]) = 0;
		RV = 0;
	}
	else
	{
		Val_char(arg[1]) = c;
		RV = 1;
	}
}

static void xfwrite_char (Func_t *func, void *rval, void **arg)
{
	RV = io_nputc(Val_char(arg[1]), Val_io(arg[0]), 1);
}
#endif

static void xfread_str (Func_t *func, void *rval, void **arg)
{
	int byte;
	ulong_t size;
	io_t *io;

	io = Val_io(arg[0]);
	memfree(Val_str(arg[1]));
	Val_str(arg[1]) = NULL;
	RV = 0;
	byte = io_getc(io);

	if	(byte == EOF)	RV = 0;
	else if	(byte == 0)	RV = 1;
	else
	{
		size = io_getbyte(io, byte);
		Val_str(arg[1]) = io_mread(io, size);
		RV = 1 + byte + size;
	}
}

static void xfwrite_str (Func_t *func, void *rval, void **arg)
{
	RV = io_putstr(Val_str(arg[1]), Val_io(arg[0]));
}

IOFUNC(fread_any, get_MSBF(io, arg[1], func->arg[1].type->size));
IOFUNC(rfread_any, get_LSBF(io, arg[1], func->arg[1].type->size));
IOFUNC(fwrite_any, put_MSBF(io, arg[1], func->arg[1].type->size));
IOFUNC(rfwrite_any, put_LSBF(io, arg[1], func->arg[1].type->size));


/*	Tabelle mit IO-Funktionen
*/

static FuncDef_t tab_IoFunc[] = {
	{ FUNC_VIRTUAL, &Type_int, "fread (IO, _Ptr_ &)", dummy_IoFunc },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, _Ptr_ &)", dummy_IoFunc },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, _Ptr_)", dummy_IoFunc },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, _Ptr_)", dummy_IoFunc },
	
	{ FUNC_VIRTUAL, &Type_int, "fread (IO, bool &)", xfread_bool },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, bool &)", xfread_bool },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, bool *)", xfwrite_bool },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, bool *)", xfwrite_bool },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, str &)", xfread_str },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, str &)", xfread_str },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, str)", xfwrite_str },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, str)", xfwrite_str },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, char &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, char &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, char)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, char)", fwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, byte &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, byte &)", rfread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, byte)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, byte)", rfwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, short &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, short &)", rfread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, short)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, short)", rfwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, int &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, int &)", rfread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, int)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, int)", rfwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, long &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, long &)", rfread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, long)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, long)", rfwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, double &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, double &)", fread_any },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, double)", fwrite_any },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, double)", fwrite_any },

	{ FUNC_VIRTUAL, &Type_int, "fread (IO, . &)", fread_obj },
	{ FUNC_VIRTUAL, &Type_int, "rfread (IO, . &)", rfread_obj },
	{ FUNC_VIRTUAL, &Type_int, "fwrite (IO, .)", fwrite_obj },
	{ FUNC_VIRTUAL, &Type_int, "rfwrite (IO, .)", rfwrite_obj },
};


void IoFuncDef(void)
{
	AddFuncDef(tab_IoFunc, tabsize(tab_IoFunc));
}


/*	Ladefunktion bestimmen
*/

Func_t *md_loadfunc(const Type_t *type)
{
	return get_iofunc(type, md_revbyteorder ? "rfread" : "fread", 1);
}


/*	Ausgabefunktion bestimmen
*/

Func_t *md_savefunc(const Type_t *type)
{
	return get_iofunc(type, "fwrite", 0);
}


/*	Ladefunktion bestimmen
*/

Func_t *get_iofunc(const Type_t *type, const char *name, int flag)
{
	Func_t *func;

	func = GetFunc(&Type_int, GetGlobalFunc(name), 2, &Type_io, 0, type, flag);

	if	(func == NULL)
	{
		reg_set(1, type2str(type));
		liberror(MSG_MDMAT, flag ? 22 : 24);
	}

	return func;
}
