/*
Parser für Variablendefinitionen

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
#include <EFEU/stdtype.h>
#include <EFEU/Op.h>

static void d_vdef (const EfiType *type, void *tg, int mode);
static void c_vdef (const EfiType *type, void *tg, const void *src);
static EfiObj *e_vdef(const EfiType *type, const void *ptr);

typedef struct {
	EfiType *type;	/* Variablentype */
	EfiName name;	/* Namensdefinition */
	unsigned cnst;	/*  Flag für Konstante */
	EfiObjList *idx; /* Indexliste */
	EfiObj *defval;	/* Initialisierungswert */
} EfiVarDecl;

EfiType Type_vdef = EVAL_TYPE("_VarDecl_", EfiVarDecl, e_vdef, d_vdef, c_vdef);

/*	Scope-Namen lesen
*/

EfiName *Parse_sname (IO *io, EfiName *buf)
{
	int c;

	if	(buf == NULL)	buf = &Buf_name;

	buf->obj = NULL;
	buf->name = io_getname(io);

	if	(buf->name == NULL)	return NULL;

	c = io_eat(io, " \t");

	if	(c == ':')
	{
		c = io_getc(io);

		if	(io_peek(io) == ':')
		{
			EfiType *type;

			if	(buf->name == NULL)
			{
				buf->obj = NewPtrObj(&Type_vtab,
					RefVarTab(GlobalVar));
			}
			else if	((type = GetType(buf->name)) != NULL)
			{
				buf->obj = type2Obj(type);
				memfree(buf->name);
			}
			else if	(buf->obj == NULL)
			{
				buf->obj = NewPtrObj(&Type_name, buf->name);
			}
			else	buf->obj = NewObj(&Type_sname, buf);

			io_getc(io);
			buf->name = io_getname(io);
		}
		else	io_ungetc(c, io);
	}

	if	(buf->name && strcmp(buf->name, "operator") == 0)
	{
		memfree(buf->name);

		if	(io_eat(io, " \t") == '"')
		{
			io_scan(io, SCAN_STR, (void **) &buf->name);
		}
		else	buf->name = io_mgets(io, "%s");
	}

	if	(buf->name)	return buf;

	if	(buf->obj)
	{
		UnrefObj(buf->obj);
		io_error(io, "[efmain:81]", NULL);
	}

	return NULL;
}


/*	Index lesen
*/

EfiObjList *Parse_idx (IO *io)
{
	EfiObjList *list;
	EfiObjList **ptr;
	int c;

	list = NULL;
	ptr = &list;

	while ((c = io_eat(io, " \t")) == '[')
	{
		io_getc(io);
		*ptr = NewObjList(Parse_index(io));
		ptr = &(*ptr)->next;
	}

	return list;
}


/*	Variablendefinition lesen
*/

static EfiType *get_type (IO *io, char *name)
{
	EfiType *type;

	if	((type = GetType(name)) == NULL)
		io_error(io, "[efmain:83]", "s", name);

	return Parse_type(io, type);
}


EfiObj *Parse_vdef (IO *io, EfiType *type, int flag)
{
	EfiVarDecl vdef;
	EfiObjList **ptr;
	EfiObj *obj;
	int c;

/*	Datentype bestimmen
*/
	while (type == NULL)
	{
		char *tname;

		if	((tname = io_getname(io)) == NULL)
		{
			io_error(io, "[efmain:82]", NULL);
			return NULL;
		}

		if	(strcmp("const", tname) == 0)
		{
			flag |= VDEF_CONST;
		}
		else if	(strcmp("typeof", tname) == 0)
		{
			EfiObj *obj = EvalObj(Parse_term(io, 0), NULL);
			type = obj ? obj->type : NULL;
			UnrefObj(obj);
		}
		else if	((type = get_type(io, tname)) == NULL)
		{
			return NULL;
		}

		memfree(tname);
	}

	vdef.type = type;
	vdef.cnst = (flag & VDEF_CONST) ? 1 : 0;

/*	Variablenname bestimmen
*/
	if	(Parse_sname(io, &vdef.name) == NULL)
		return NULL;

	c = io_eat(io, " \t");

	if	(c == '&' || c == '(')
		return Parse_func(io, type, &vdef.name,
			(flag & VDEF_LVAL) ? FUNC_LRETVAL : 0);

/*	Indexliste generieren
*/
	vdef.idx = NULL;
	ptr = &vdef.idx;
	
	while (c == '[')
	{
		io_getc(io);
		*ptr = NewObjList(Parse_index(io));
		ptr = &(*ptr)->next;
		c = io_eat(io, " \t");
	}

/*	Initialisierer bestimmen
*/
	if	(c == '=')
	{
		io_getc(io);
		vdef.defval = Parse_term(io, OpPrior_Assign);
		c = io_eat(io, " \t");
	}
	else	vdef.defval = NULL;

	obj = NewObj(&Type_vdef, &vdef);

/*	Bei VDEF_REPEAT: Weitere Variablen zum gleichen Type bestimmen
*/
	if	((flag & VDEF_REPEAT) && c == ',')
	{
		io_getc(io);
		obj = CommaTerm(obj, Parse_vdef(io, type, flag));
	}

	return obj;
}


EfiObjList *VarDefList (IO *io, int delim)
{
	EfiObjList *list, **ptr;
	EfiType *type;
	EfiObj *obj;
	char *err;
	int c;

	list = NULL;
	ptr = &list;
	err = NULL;

	while (!err && (c = io_eat(io, "%s;")) != delim)
	{
		if	(c == EOF)
		{
			err = "[efmain:121]";
			break;
		}

		if	((obj = Parse_obj(io, SCAN_NAME)) == NULL)
		{
			err = "[efmain:122]";
			break;
		}

		if	(obj->type != &Type_type)
		{
			err = "[efmain:122]";
			UnrefObj(obj);
			break;
		}

		type = Val_type(obj->data);
		UnrefObj(obj);

		for (;;)
		{
			obj = Parse_vdef(io, type, 0);

			if	(obj == NULL)
			{
				err = "[efmain:122]";
				break;
			}

			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
			c = io_eat(io, " \t");

			if	(c == ',')
			{
				io_getc(io);
			}
			else	break;
		}
	}

	if	(err)
	{
		io_error(io, err, NULL);
		DelObjList(list);
		return NULL;
	}

	return list;
}


static void d_vdef (const EfiType *type, void *data, int mode)
{
	EfiVarDecl *tg = data;
	UnrefObj(tg->name.obj);
	memfree(tg->name.name);
	DelObjList(tg->idx);
	UnrefObj(tg->defval);
	memset(tg, 0, sizeof(EfiVarDecl));
}


static void c_vdef (const EfiType *type, void *tptr, const void *sptr)
{
	EfiVarDecl *tg = tptr;
	const EfiVarDecl *src = sptr;
	tg->type = src->type;
	tg->name.obj = RefObj(src->name.obj);
	tg->name.name = mstrcpy(src->name.name);
	tg->idx = RefObjList(src->idx);
	tg->defval = RefObj(src->defval);
}


static void vec_clean (VarTabEntry *entry)
{
	memfree((char *) entry->name);
	lfree(entry->data);
}

/*	Variablendefinition auswerten
*/

static EfiObj *e_vdef(const EfiType *type, const void *ptr)
{
	const EfiVarDecl *vdef;
	VarTabEntry *entry;
	EfiObj *obj, *defval;
	EfiType *vartype;
	EfiVarTab *vtab;
	char *name;
	size_t dim;

	vdef = ptr;
	vartype = vdef->type;
	defval = EvalObj(RefObj(vdef->defval), NULL);
	name = vdef->name.name;
	vtab = Obj2Ptr(RefObj(vdef->name.obj), &Type_vtab);

	if	(vdef->idx)
	{
		defval = EvalObj(defval, &Type_list);
		vartype = VecType(vartype, vdef->idx->next);
		obj = EvalObj(RefObj(vdef->idx->obj), &Type_int);

		if	(obj)	
		{
			dim = Val_int(obj->data);
			UnrefObj(obj);
		}
		else if	(defval)
		{
			dim = ObjListLen(Val_list(defval->data));
		}
		else	dim = 0;
		
		if	(dim == 0)
		{
			dbg_note(NULL, "[efmain:302]", NULL);
			return NULL;
		}
	}
	else if	(vdef->cnst)
	{
		defval = EvalObj(defval, vartype);

		if	(defval)
		{
			obj = ConstObj(vartype, defval->data);
			UnrefObj(defval);
			defval = NULL;
		}
		else	obj = NewObj(vartype, NULL);

		VarTab_xadd(vtab, mstrcpy(name), NULL, RefObj(obj));
		return obj;
	}
	else	dim = 0;

	vtab = CurrentVarTab(vtab);
	entry = VarTab_get(vtab, name);

	if	(entry)
	{
		if	(entry->type != vartype)
			dbg_note(NULL, "[efmain:154b]", "s", name);

		if	(entry->get)
			obj = entry->get(NULL, entry->data);
		else	obj = RefObj(entry->obj);
	}
	else if (dim)
	{
		VarTabEntry entry;
		entry.name = mstrcpy(name);
		entry.desc = NULL;
		entry.type = vartype;
		entry.get = NULL;
		entry.entry_clean = vec_clean;
		entry.data = lmalloc(vartype->size * dim);
		memset(entry.data, 0, vartype->size * dim);
		entry.obj = EfiVecObj(vartype, entry.data, dim);
		obj = RefObj(entry.obj);
		VarTab_add(vtab, &entry);
	}
	else
	{
		obj = LvalObj(NULL, vartype);
		VarTab_xadd(vtab, mstrcpy(name), NULL, RefObj(obj));
	}

	if	(defval)
		return AssignObj(obj, defval);

	return obj;
}
