/*
Indizes und Vektoren

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
#include <EFEU/printobj.h>

#define	VEC_BSIZE	128	/* Blockgröße für Zahl der Datenbankeinträge */

static ALLOCTAB(vec_tab, 0, sizeof(EfiVec));

static char *vec_ident (const void *data)
{
	const EfiVec *vec = data;
	return msprintf("%s[%d]", vec->type->name, vec->buf.used);
}

static void vec_clean (void *data)
{
	EfiVec *vec;
	char *ptr;
	size_t n;

	vec = data;
	ptr = vec->buf.data;

	if	(vec->buf.blksize)
	{
		for (n = vec->buf.used; n-- > 0; ptr += vec->type->size)
			DestroyData(vec->type, ptr);

		vb_free(&vec->buf);
	}

	del_data(&vec_tab, vec);
}


static RefType vec_reftype = REFTYPE_INIT("Vec", vec_ident, vec_clean);

static void vec_realloc (EfiVec *vec, size_t dim)
{
	vb_realloc(&vec->buf, dim);

	while (vec->buf.used < dim)
		CopyData(vec->type, vb_next(&vec->buf), vec->type->defval);
}

static int test_vla (EfiVec *vec)
{
	if	(!vec)
		dbg_note(NULL, "[efmain:255]", NULL);
	else if	(vec->buf.blksize == 0)
		dbg_note(NULL, "[efmain:256]", NULL);
	else	return 1;

	return 0;
}

void EfiVec_resize (EfiVec *vec, size_t dim)
{
	if	(!test_vla(vec))
	{
		return;
	}
	else if	(vec->buf.used > dim)
	{
		rd_debug(vec, "reduce %lu -> %lu",
			(unsigned long) vec->buf.used, (unsigned long) dim);

		do
		{
			vec->buf.used--;
			DestroyData(vec->type, (char *) vec->buf.data
				+ vec->buf.elsize * vec->buf.used);
		}
		while	(vec->buf.used > dim);
	}
	else if	(vec->buf.used < dim)
	{
		rd_debug(vec, "expand %lu -> %lu",
			(unsigned long) vec->buf.used, (unsigned long) dim);
		vec_realloc(vec, dim);
	}
}

void EfiVec_append (EfiVec *vec, EfiObj *obj)
{
	if	(test_vla(vec) && (obj = EvalObj(RefObj(obj), vec->type)))
	{
		CopyData(vec->type, vb_next(&vec->buf), obj->data);
		UnrefObj(obj);
	}
}

void EfiVec_delete (EfiVec *vec, size_t pos, size_t dim)
{
	void *data;

	if	(!test_vla(vec))	return;

	if	(pos + dim > vec->buf.used)
	{
		dbg_note(NULL, "[efmain:155]", NULL);

		if	(pos >= vec->buf.used)
			return;

		dim = vec->buf.used - pos;
	}

	data = vb_delete(&vec->buf, pos, dim);

	while (dim-- > 0)
	{
		DestroyData(vec->type, data);
		data = (char *) data + vec->buf.elsize;
	}
}

EfiVec *NewEfiVec (EfiType *type, void *data, size_t dim)
{
	if	(type)
	{
		EfiVec *vec = new_data(&vec_tab);
		vec->type = type;
		vec->buf.elsize = type->size;

		if	(data && dim)
		{
			vec->buf.data = data;
			vec->buf.blksize = 0;
			vec->buf.size = dim;
			vec->buf.used = dim;
		}
		else
		{
			vec->buf.data = NULL;
			vec->buf.blksize = dim ? dim : VEC_BSIZE;
			vec->buf.size = 0;
			vec->buf.used = 0;

			if	(dim)
				vec_realloc(vec, dim);
		}

		return rd_init(&vec_reftype, vec);
	}

	return NULL;
}

EfiVec *EfiVec_copy (EfiVec *s)
{
	if	(s)
	{
		EfiVec *t = NewEfiVec(s->type, NULL, s->buf.used);
		rd_debug(t, "copy %p %lu", s, (unsigned long) s->buf.used);
		CopyVecData(s->type, s->buf.used, t->buf.data, s->buf.data);
		return t;
	}

	return NULL;
}

EfiObj *EfiVecObj (EfiType *type, void *data, size_t dim)
{
	return NewPtrObj(&Type_vec, NewEfiVec(type, data, dim));
}

size_t Read_vec (const EfiType *st, void *data, IO *io)
{
	EfiVec *vec;
	size_t recl;
	size_t n;
	
	vec = Val_ptr(data);
	n = io_read_size(&recl, io);
	EfiVec_resize(vec, recl);

	if	(recl && vec && recl <= vec->buf.used)
	{
		vb_realloc(&vec->buf, recl);
		n += ReadVecData(vec->type, recl, vec->buf.data, io);
	}

	return n;
}

size_t Write_vec (const EfiType *st, const void *data, IO *io)
{
	EfiVec *vec = Val_ptr(data);
	size_t n = io_write_size(vec ? vec->buf.used : 0, io);

	if	(vec && vec->buf.used)
		n += WriteVecData(vec->type,
			vec->buf.used, vec->buf.data, io);

	return n;
}

int Print_vec (const EfiType *st, const void *data, IO *io)
{
	EfiVec *vec = Val_ptr(data);

	return (vec && vec->buf.used) ?
		PrintVecData(io, vec->type, vec->buf.data, vec->buf.used) :
		io_puts("{ }", io);
}

EfiType Type_vec = IOREF_TYPE("EfiVec", EfiVec *, Read_vec, Write_vec);


EfiObj *Vector (EfiVec *vec, size_t idx)
{
	void *ptr;

	if	(!vec)
	{
		dbg_note(NULL, "[efmain:255]", NULL);
		return NULL;
	}

	if	(idx >= vec->buf.used)
	{
		if	(!vec->buf.blksize)
		{
			dbg_note(NULL, "[efmain:155]", NULL);
			return NULL;
		}
		else	EfiVec_resize(vec, idx + 1);
	}

	ptr = (char *) vec->buf.data + idx * vec->type->size;

	if	(vec->type->list && vec->type->dim && !vec->type->list->next)
	{
		EfiVar *var = vec->type->list;
		vec = NewEfiVec(var->type, ptr, var->dim);
		return NewPtrObj(&Type_vec, vec);
	}

	return LvalObj(&Lval_ref, vec->type, vec, ptr);
}
