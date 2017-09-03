/*
:*:JSON compatible data structure

$Copyright (C) 2017 Erich Frühstück
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

#include <EfiType/Json.h>
#include <EFEU/stdtype.h>
#include <EFEU/printobj.h>

#define	Val_json(x)	((Json **) (x))[0]

static ALLOCTAB(json_alloc, "Json", 0, sizeof(Json));

static void json_push(Json **root, Json *json)
{
	if	(*root)
	{
		json->prev = (*root)->prev;
		(*root)->prev = json;
		json->next = json->prev->next;
		json->prev->next = json;
	}
	else
	{
		json->prev = json;
		json->next = json;
		*root = json;
	}
}

static Json *json_pop(Json **root)
{
	Json *json = *root;

	if	(json == NULL)
		return NULL;

	if	(json->next != json)
	{
		*root = json->next;
		json->prev->next = json->next;
		json->next->prev = json->prev;
	}
	else
	{
		*root = NULL;
	}

	json->next = NULL;
	json->prev = NULL;
	return json;
}

static int json_size(Json *root)
{
	int n = 0;
	Json *json = root;

	if	(json)
	{
		do
		{
			++n;
			json = json->next;
		}
		while (json != root);
	}

	return n;
}

static Json *json_get(Json *root, const char *name)
{
	Json *json = root;

	if	(json)
	{
		do
		{
			if	(mstrcmp(json->name, name) == 0)
				return json;

			json = json->next;
		}
		while (json != root);
	}

	return NULL;
}

static int print_data(IO *io, const EfiType *type, const void *data);

static int print_vec(IO *io, const EfiType *type, const void *data, size_t dim)
{
	char *delim;
	int i, n;

	n = io_puts("[", io);
	delim = NULL;

	for (i = 0; i < dim; i++)
	{
		n += io_puts(delim, io);
		n += print_data(io, type, data);
		data = (const char *) data + type->size;
		delim = ",";
	}

	n += io_puts("]", io);
	return n;
}

int print_data(IO *io, const EfiType *type, const void *data)
{
	if	(IsTypeClass(type, &Type_vec))
	{
		EfiVec *vec = Val_ptr(data);
		return print_vec(io, vec->type, vec->buf.data, vec->buf.used);
	}

	if	(type->dim)
		return print_vec(io, type->base, data, type->dim);

	return PrintData(io, type, data);
}

static int json_print (const EfiType *st, const void *data, IO *io)
{
	const Json *json = Val_json(data);

	if	(json)
	{
		int n = io_puts("{", io);
		const Json *x = json;

		do
		{
			if	(x != json)
				n += io_puts(",", io);

			n += io_puts("\"", io);
			n += io_xputs(x->name, io, "\"");
			n += io_puts("\":", io);
			n += print_data(io, x->obj->type, x->obj->data);
			x = x->next;
		}
		while (x != json);

		n += io_puts("}", io);
		return n;
	}
	else	return io_puts("{}", io);
}

static void json_clear(const EfiType *st, void *data, int mode)
{
	Json *json;

	while ((json = json_pop(data)))
	{
//		fprintf(stderr, "delete %p\n", json);
		memfree(json->name);
		UnrefObj(json->obj);
		del_data(&json_alloc, json);
	}
}

static void json_copy(const EfiType *st, void *tg, const void *src_ptr)
{
	const Json *src = ((void **) src_ptr)[0];
	const Json *s = src;

	if	(src == NULL)
		return;

	do
	{
		Json *json = new_data(&json_alloc);
		json->name = mstrcpy(s->name);
		json->obj = LvalObj(NULL, s->obj->type);
		CopyData(json->obj->type, json->obj->data, s->obj->data);
		json_push(tg, json);
//		fprintf(stderr, "new %p\n", json);
		s = s->next;
	}
	while (s != src);
}

EfiType Type_Json = PTR_TYPE("Json", Json, &Type_ptr,
	json_print, json_clear, json_copy);

static void json_parse_list(Json **root, IO *in);

static EfiObj *json_parse_obj(IO *in)
{
	EfiObj *obj;
	int c = io_eat(in, "%s");

	if	(c == '{')
	{
		io_getc(in);
		obj = LvalObj(NULL, &Type_Json);
		json_parse_list(obj->data, in);

		if	(io_eat(in, "%s") == '}')
			io_getc(in);

		return obj;
	}
	else if ((obj = Parse_obj(in, SCAN_ANYVAL|SCAN_STR)))
	{
		EfiObj *tg = LvalObj(NULL, obj->type);
		AssignObj(tg, obj);
		return tg;
	}
	else
	{
		return LvalObj(NULL, &Type_ptr);
	}
}

static EfiObj *json_parse_vec(IO *in)
{
	EfiObj *obj = NULL;
	EfiType *type = NULL;
	EfiVec *vec = NULL;
	int n = 0;
	EfiObj *o;
	
	while ((o = json_parse_obj(in)))
	{
		if	(!vec)
		{
			type = NewVecType(o->type, 0);
			obj = LvalObj(NULL, type);
			vec = Val_ptr(obj->data);
		}

		UnrefEval(AssignObj(Vector(vec, n++), o));

		if	((io_eat(in, "%s") != ','))
			break;

		io_getc(in);
	}

	if	(!obj)
		obj = LvalObj(NULL, &Type_ptr);

	return obj;
}
	
static Json *json_parse_var(IO *in)
{
	char *name = NULL;
	Json *json;
	int c;

	if	(io_eat(in, "%s") != '"')
		return NULL;

	io_scan(in, SCAN_STR, (void **) &name);

	if	(io_eat(in, "%s") != ':')
		return NULL;

	io_getc(in);
	c = io_eat(in, "%s");
	json = new_data(&json_alloc);
	json->name = mstrcpy(name);

	if	(c == '[')
	{
		io_getc(in);
		json->obj = json_parse_vec(in);

		if	((io_eat(in, "%s") == ']'))
			io_getc(in);
	}
	else
	{
		json->obj = json_parse_obj(in);
	}

	return json;
}

static void json_parse_list(Json **root, IO *in)
{
	Json *json;
	
	while ((json = json_parse_var(in)))
	{
		json_push(root, json);

		if	((io_eat(in, "%s") != ','))
			break;

		io_getc(in);
	}
}

static void json_parse(Json **root, IO *in)
{
	if	(io_eat(in, "%s") == '{')
	{
		io_getc(in);
		json_clear(&Type_Json, root, 0);
		json_parse_list(root, in);

		if	(io_eat(in, "%s") == '}')
			io_getc(in);
	}
	else
	{
		json_parse_list(root, in);
	}
}

static void func_push_back (EfiFunc *func, void *rval, void **arg)
{
	Json *json = new_data(&json_alloc);
	json->name = mstrcpy(Val_str(arg[1]));
	json->obj = LvalObj(NULL, Val_type(arg[2]));
	json_push(arg[0], json);
	Val_obj(rval) = RefObj(json->obj);
//	fprintf(stderr, "new %p\n", json);
}

static void func_push_front (EfiFunc *func, void *rval, void **arg)
{
	func_push_back(func, rval, arg);
	Json *root = Val_json(arg[0]);
	Val_json(arg[0]) = root->prev;
}

static void func_parse (EfiFunc *func, void *rval, void **arg)
{
	json_parse(arg[0], Val_io(arg[1]));
}

static void func_str2json (EfiFunc *func, void *rval, void **arg)
{
	IO *in = io_mstr(Val_str(arg[0]));
	json_parse(rval, in);
	io_close(in);
}

static void func_data (EfiFunc *func, void *rval, void **arg)
{
	Json *json = json_get(Val_json(arg[0]), Val_str(arg[1]));
	Val_obj(rval) = json ? RefObj(json->obj) : NULL;
}

static void func_clear (EfiFunc *func, void *rval, void **arg)
{
	json_clear(&Type_Json, arg[0], 0);
	Val_json(arg[0]) = NULL;
}

static void func_empty (EfiFunc *func, void *rval, void **arg)
{
	Val_bool(rval) = (Val_json(arg[0]) == NULL);
}

static void func_size (EfiFunc *func, void *rval, void **arg)
{
	Val_int(rval) = json_size(Val_json(arg[0]));
}

static EfiFuncDef json_ftab[] = {
	{ 0, &Type_obj, "Json::push_back (str name, Type_t type)",
		func_push_back },
	{ 0, &Type_obj, "Json::push_front (str name, Type_t type)",
		func_push_front },
	{ 0, &Type_obj, "Json::data (str name)", func_data },
	{ FUNC_VIRTUAL, &Type_obj, "operator[] (Json *, str name)", func_data },
	{ 0, &Type_void, "Json::parse (IO io)", func_parse },
	{ 0, &Type_Json, "str ()", func_str2json },
	{ 0, &Type_void, "Json::clear ()", func_clear },
	{ 0, &Type_bool, "Json::empty ()", func_empty },
	{ 0, &Type_int, "Json::size ()", func_size },
};

void Json_setup (void)
{
	static int setup_done = 0;
	EfiType *type;

	if	(setup_done)	return;

	setup_done = 1;
	type = &Type_Json;
	AddType(type);
	AddFuncDef(json_ftab, tabsize(json_ftab));
}

