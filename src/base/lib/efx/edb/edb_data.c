/*
Daten einlesen
*/

#include <EFEU/EDB.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/DBData.h>
#include <EFEU/parsearg.h>
#include <ctype.h>

static void vec_data (EfiType *type, char *data, size_t dim, EfiObjList *list)
{
	while (dim > 0)
	{
		if	(list)
		{
			EfiObj *obj = EvalObj(RefObj(list->obj), type);
			list = list->next;

			if	(obj)
			{
				CopyData(type, data, obj->data);
				UnrefObj(obj);
			}
			else	CleanData(type, data);
		}
		else	CleanData(type, data);

		data += type->size;
		dim--;
	}
}

static void set_var (EfiType *type, unsigned dim, char *data, EfiObj *obj)
{
	obj = EvalObj(obj, NULL);

	if	(!obj)
	{
		if	(dim)
			CleanVecData(type, dim, data);
		else	CleanData(type, data);
	}
	else if	(dim)
	{
		if	(obj->type != &Type_list)
			obj = NewPtrObj(&Type_list, NewObjList(obj));

		vec_data(type, data, dim, Val_list(obj->data));
	}
	else if	(type->dim)
	{
		if	(obj->type != &Type_list)
			obj = NewPtrObj(&Type_list, NewObjList(obj));

		vec_data(type->base, data, type->dim, Val_list(obj->data));
	}
	else if	(type->base == &Type_vec)
	{
		if	(obj->type != &Type_list)
			obj = NewPtrObj(&Type_list, NewObjList(obj));

		if	((obj = EvalObj(obj, type)))
			CopyData(type, data, obj->data);
		else	CleanData(type, data);
	}
	else if	((obj = EvalObj(obj, type)))
	{
		CopyData(type, data, obj->data);
	}
	else	CleanData(type, data);

	UnrefObj(obj);
}

static int start_line (IO *io)
{
	int c;

	while ((c = io_mgetc(io, 1)) != EOF)
	{
		if	(c == '/' && iocpy_cskip(io, NULL, c,
				NULL, 1) != EOF)
		{
			continue;
		}
		else if	(c == '#')
		{
			do	c = io_mgetc(io, 1);
			while	(c != '\n' && c != EOF);
		}
		else if	(!isspace(c))
		{
			io_ungetc(c, io);
			break;
		}
	}

	return c;
}

static int read_binary (EfiType *type, void *data, void *par)
{
	if	(io_peek(par) == EOF)
		return 0;

	return ReadData(type, data, par) != 0;
}

static EfiObj *label_obj (char *label, int flag)
{
	return flag ? strterm(label) : str2Obj(mstrcpy(label));
}

static int read_label (EfiType *type, void *data, void *par)
{
	DBData *db;
	EfiVar *var;
	EfiObj *obj;
	char *p;
	IO *io;
	int i;

	io = par;

	if	(start_line(io) == EOF)
		return 0;

	if	((db = DBData_qtext(NULL, io, ";\t")) == NULL)
		return 0;

	if	(!type->list)
	{
		obj = label_obj(DBData_field(db, 1), type->dim != 0);
		set_var(type, 0, data, obj);
		return 1;
	}

	for (i = 1, var = type->list; var != NULL; var = var->next, i++)
	{
		p = (char *) data + var->offset;
		obj = label_obj(DBData_field(db, i),
			var->dim || var->type->dim);
		set_var(var->type, var->dim, p, obj);
	}

	return 1;
}

static int read_obj (EfiType *type, void *data, void *par)
{
	IO *io = par;

	if	(start_line(io) == EOF)
		return 0;

	set_var(type, 0, data, Parse_term(io, 0));
	io_getc(io);
	return 1;
}

static int read_list (EfiType *type, void *data, void *par)
{
	EfiObj *obj;
	EfiVar *var;
	char *p;
	IO *io;
	int c;

	io = par;

	if	((c = start_line(io)) == EOF)
		return 0;

	var = type->list;

	while (c != '\n' && c != EOF)
	{
		obj = Parse_term(io, 0);

		if	(var)
		{
			char *p = (char *) data + var->offset;
			set_var(var->type, var->dim, p, obj);
			var = var->next;
		}
		else	UnrefObj(obj);

		c = io_getc(io);
	}

	while (var)
	{
		p = (char *) data + var->offset;

		if	(var->dim)
			CleanVecData(var->type, var->dim, p);
		else	CleanData(var->type, p);

		var = var->next;
	}

	return 1;
}

void edb_data (EDB *edb, IO *io, const char *def)
{
	char *p;
	char *buf;
	const char *mode;
	AssignArg *arg;

	edb->save = 0;
	buf = NULL;

	if	(def && (p = strchr(def, ',')))
	{
		mode = buf = mstrncpy(def, p - def);
		def = p + 1;
	}
	else
	{
		mode = def;
		def = NULL;
	}

	while ((arg = assignarg(def, &p, ",")))
	{
		if	(mstrcmp("skip", arg->name) == 0)
		{
			unsigned n = strtoul(arg->arg, NULL, 10);

			while (n--)
			{
				int c;

				do	c = io_getc(io);
				while	(c != '\n' && c != EOF);
			}
		}

		memfree(arg);
		def = p;
	}

	if	(mstrcmp(mode, "binary") == 0)
	{
		edb_input(edb, read_binary, io);
	}
	else if	(mstrcmp(mode, "label") == 0)
	{
		edb_input(edb, read_label, io);
	}
	else if	(edb->obj && edb->obj->type->list)
	{
		edb_input(edb, read_list, io);
	}
	else	edb_input(edb, read_obj, io);
}
