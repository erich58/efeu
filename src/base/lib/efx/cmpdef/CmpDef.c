/*
:*:	compare definition
:de:	Vergleichsdefinition

$Copyright (C) 2005 Erich Frühstück
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

#include <EFEU/CmpDef.h>
#include <ctype.h>

#define	ID	"CmpDef"

static ALLOCTAB(entry_tab, 0, sizeof(CmpDefEntry));
static STRBUF(name_buf, 0);

typedef struct {
	EfiObj *base;
	EfiStruct *st;
	CmpDefEntry *entry;
} CDEF;

static char *parse_name (const char *def, char **ptr)
{
	sb_clean(&name_buf);

	while (*def == '_' || isalnum((unsigned char) *def))
		sb_putc(*def++, &name_buf);

	if	(ptr)
		*ptr = (char *) def;

	sb_putc(0, &name_buf);
	return (char *) name_buf.data;
}

static void put_entry (CmpDefEntry *p, IO *io)
{
	io_putc(p->invert ? '-' : '+', io);
	io_printf(io, "%d:%d*%d %s", p->offset, p->dim,
		p->type->size, p->type->name);

	if	(p->clean == rd_deref)
	{
		char *x = rd_ident(p->par);

		io_puts(" {", io);
		io_puts(x, io);
		io_puts("}", io);
		memfree(x);
	}
	else	io_printf(io, " {%p}", p->par);
}

static char *cmp_ident (const void *data)
{
	const CmpDef *cdef = data;

	if	(cdef->type)
	{
		StrBuf *sb;
		CmpDefEntry *p;
		IO *io;
		
		sb = sb_create(0);
		io = io_strbuf(sb);
		io_puts(cdef->type->name, io);

		for (p = cdef->list; p; p = p->next)
		{
			io_putc(' ', io);
			put_entry(p, io);
		}

		io_close(io);
		return sb2str(sb);
	}

	return NULL;
}

static void entry_clean (CmpDefEntry *entry)
{
	if	(entry)
	{
		entry_clean(entry->next);

		if (entry->clean) entry->clean(entry->par);

		del_data(&entry_tab, entry);
	}
}

static void cmp_clean (void *data)
{
	CmpDef *cmp = data;
	entry_clean(cmp->list);
	memfree(cmp);
}

static RefType CmpDef_reftype = REFTYPE_INIT(ID, cmp_ident, cmp_clean);

#define	FMT_GET	"CmpDef: virtual function $1() not found.\n"
#define	FMT_CMP	"CmpDef: function $1() not defined for type \"$2\".\n"
#define	FMT_VAR	"CmpDef: $1: undefined member \"$2\".\n"
#define	FMT_KEY	"CmpDef: unexpected character $1 in compare defination.\n"
#define	FMT_DIM	"CmpDef: vector index $1 out of range.\n"

static int do_cmp_func (CmpDefEntry *entry, void *a, void *b)
{
	EfiFunc *func = entry->par;
	void *args[2];
	int r;
	int i;

	args[0] = a;
	args[1] = b;

	r = 0;

	for (i = 0; i < entry->dim; i++)
	{
		func->eval(func, &r, args);

		if	(r)	return r;

		args[0] = (char *) args[0] + entry->type->size;
		args[1] = (char *) args[1] + entry->type->size;
	}

	return 0;
}

static int do_cmp_std (CmpDefEntry *entry, void *a, void *b)
{
	return memcmp(a, b, entry->dim * entry->type->size);
}

static int do_cmp_vla (CmpDefEntry *entry, void *a, void *b)
{
	size_t n, k;
	int r;

	for (k = 0; k < entry->dim; k++)
	{
		EfiVec *v1 = Val_ptr(a);
		EfiVec *v2 = Val_ptr(b);
		unsigned char *p1 = v1->buf.data;
		unsigned char *p2 = v2->buf.data;

		for (n = 0; n < v1->buf.used && n < v2->buf.used; n++)
		{
			if	((r = cmp_data(entry->par, p1, p2)))
				return r;

			p1 += v1->buf.elsize;
			p2 += v2->buf.elsize;
		}

		if	(v1->buf.used < v2->buf.used)	return -1;
		if	(v1->buf.used > v2->buf.used)	return 1;

		a = (char *) a + entry->type->size;
		b = (char *) b + entry->type->size;
	}

	return 0;
}

static int do_cmp_sub (CmpDefEntry *entry, void *a, void *b)
{
	size_t n;
	int r;

	for (n = 0; n < entry->dim; n++)
	{
		if	((r = cmp_data(entry->par, a, b)))
			return r;

		a = (char *) a + entry->type->size;
		b = (char *) b + entry->type->size;
	}

	return 0;
}

static EfiVirFunc *get_vfunc (char *name)
{
	EfiVirFunc *vcmp = GetGlobalFunc(name);

	if	(!vcmp)
		dbg_note(ID, FMT_GET, "s", name);

	return vcmp;
}

static void set_dim (CmpDefEntry *entry)
{
	if	(!entry->dim) entry->dim = 1;

	while	(entry->type->dim)
	{
		entry->dim *= entry->type->dim;
		entry->type = entry->type->base;
	}
}

static void set_cmp (CmpDefEntry *entry, EfiVirFunc *vcmp)
{
	if	(!vcmp)
		vcmp = get_vfunc("cmp");
		
	if	(vcmp)
	{
		entry->par = GetFunc(&Type_int, vcmp, 2,
			entry->type, 0, entry->type, 0);

		if	(entry->par)
		{
			entry->cmp = do_cmp_func;
			entry->clean = rd_deref;
		}
		else	dbg_note(ID, FMT_CMP, "ms", rd_ident(vcmp),
				entry->type->name);

		rd_deref(vcmp);
	}
	else	entry->cmp = do_cmp_std;
}

static void make_subcmp (CmpDefEntry *entry, char *def, char **ptr)
{
	EfiType *type = entry->type;

	if	(type->base == &Type_vec)
	{
		EfiVec *vec = Val_ptr(type->defval);
		type = vec->type;
		entry->cmp = do_cmp_vla;
	}
	else	entry->cmp = do_cmp_sub;

	entry->par = cmp_create(type, def, ptr);
	entry->clean = rd_deref;
}

CmpDefEntry *cmp_member (EfiStruct *st, int inv)
{
	CmpDefEntry *entry = new_data(&entry_tab);
	entry->next = NULL;
	entry->type = st->type;
	entry->invert = inv;
	entry->offset = st->offset;
	entry->dim = st->dim;
	entry->cmp = do_cmp_std;
	entry->clean = NULL;
	entry->par = NULL;

	if	(st->type)
	{
		set_dim(entry);
		set_cmp(entry, NULL);
	}

	return entry;
}

static CmpDefEntry *next_entry (EfiType *base, EfiVirFunc *vcmp,
	char *def, char **ptr);

static int cdef_cmp (CmpDefEntry *entry, void *pa, void *pb)
{
	CDEF *cdef;
	EfiObj *a, *b;
	int r;
	
	cdef = entry->par;
	cdef->base->data = pa;
	a = cdef->st->member(cdef->st, cdef->base);
	cdef->base->data = pb;
	b = cdef->st->member(cdef->st, cdef->base);

	if	(a && b)
	{
		r = cdef->entry->cmp(cdef->entry, a->data, b->data);
	}
	else if	(a)	r = 1;
	else if	(b)	r = -1;
	else		r = 0;

	UnrefObj(a);
	UnrefObj(b);
	return cdef->entry->invert ? -r : r;
}

static void cdef_clean (void *ptr)
{
	CDEF *cdef = ptr;
	UnrefObj(cdef->base);
	memfree(cdef);
}

static void set_vcmp (CmpDefEntry *entry, EfiStruct *st,
	EfiVirFunc *vcmp, char *def, char **ptr)
{
	CDEF *cdef = memalloc(sizeof *cdef);

	cdef->base = LvalObj(&Lval_ptr, entry->type, NULL);
	cdef->st = st;
	cdef->entry = next_entry(st->type, vcmp, def, ptr);

	entry->par = cdef;
	entry->cmp = cdef_cmp;
	entry->clean = cdef_clean;
}

static CmpDefEntry *next_entry (EfiType *base, EfiVirFunc *vcmp,
	char *def, char **ptr)
{
	CmpDefEntry *entry;
	EfiStruct *var;

	entry = new_data(&entry_tab);
	entry->next = NULL;
	entry->cmp = do_cmp_std;
	entry->clean = NULL;
	entry->par = NULL;

	if	(*def == '+')	entry->invert = 0, def++;
	else if	(*def == '-')	entry->invert = 1, def++;
	else			entry->invert = 0;

	entry->offset = 0;
	vcmp = NULL;

	if	(base->dim)
	{
		entry->type = base->base;
		entry->dim = base->dim;
	}
	else
	{
		entry->type = base;
		entry->dim = 0;
	}

	while (*def)
	{
		if	(*def == '.')
		{
			def++;
			continue;
		}
		else if	(entry->dim && *def == '[')
		{
			unsigned n = strtoul(def + 1, &def, 0);

			while (*def && *def++ != ']')
				;

			if	(n >= entry->dim)
			{
				dbg_note(ID, FMT_DIM, "d", n);
				break;
			}

			entry->offset += n * entry->type->size;

			if	(entry->type->dim)
			{
				entry->dim = entry->type->dim;
				entry->type = entry->type->base;
			}
			else	entry->dim = 0;
		}
		else if	(*def == '(')
		{
			set_dim(entry);
			make_subcmp(entry, def + 1, &def);

			while (*def && *def++ != ')')
				;

			if	(ptr)	*ptr = def;

			return entry;
		}
		else if	(*def == ',' || *def == '=' || *def == ')')
		{
			break;
		}
		else if	(entry->dim || entry->type->base == &Type_vec)
		{
			set_dim(entry);
			make_subcmp(entry, def, ptr);
			return entry;
		}
		else if (*def == '_' || isalpha(*def))
		{
			char *name = parse_name(def, &def);

			if	(*def == ':')
			{
				vcmp = get_vfunc(name);
				def++;
				continue;
			}

			for (var = entry->type->list; var; var = var->next)
				if	(mstrcmp(var->name, name) == 0)
					break;

			if	(!var)
			{
				dbg_note(ID, FMT_VAR, "ss",
					entry->type->name, name);
				break;
			}

			entry->type = var->type;

			if	(var->member)
			{
				set_dim(entry);
				set_vcmp(entry, var, vcmp, def, ptr);
				return entry;
			}
			else
			{
				entry->offset += var->offset;
				entry->dim = var->dim;

				if	(entry->type->dim && !entry->dim)
				{
					entry->dim = entry->type->dim;
					entry->type = entry->type->base;
				}
			}
		}
		else
		{
			dbg_note(ID, FMT_KEY, "c", *def);
			def++;
		}
	}

	if	(ptr)
		*ptr = def;

	set_dim(entry);
	set_cmp(entry, vcmp);
	return entry;
}

CmpDef *cmp_alloc (EfiType *type, CmpDefEntry *list)
{
	if	(type)
	{
		CmpDef *cmp = memalloc(sizeof *cmp);
		cmp->type = type;
		cmp->list = list;
		return rd_init(&CmpDef_reftype, cmp);
	}
	else	return NULL;
}

CmpDef *cmp_create (EfiType *type, const char *def, char **cptr)
{
	CmpDef *cmp;
	CmpDefEntry **ptr;
	char *p;

	if	(!type)	return NULL;

	cmp = cmp_alloc(type, NULL);
	ptr = &cmp->list;
	p = def ? (char *) def : "";

	while (*p != 0)
	{
		if ((*ptr = next_entry(type, NULL, p, &p)))
			ptr = &(*ptr)->next;

		if	(*p == ',')	p++;
		else			break;
	}

	if	(cptr)
		*cptr = p;

	return cmp;
}

int cmp_data (CmpDef *cmp, const void *a, const void *b)
{
	CmpDefEntry *entry = cmp ? cmp->list : NULL;

	while (entry)
	{
		int r = entry->cmp(entry, (char *) a + entry->offset,
			(char *) b + entry->offset);

		if	(r)	return entry->invert ? -r : r;

		entry = entry->next;
	}

	return 0;
}
