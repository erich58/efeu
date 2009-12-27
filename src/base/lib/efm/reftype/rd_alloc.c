/*
:*:dynamic allocatetd memory with reference counter
:de:Dynamische Speicheranforderung mit Referenzzähler

$Copyright (C) 2006 Erich Frühstück
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

#include <EFEU/memalloc.h>
#include <EFEU/refdata.h>

typedef struct {
	REFVAR;
	void (*clean) (void *data);
	void *data;
} REFPTR;

static void mem_clean (void *data)
{
	REFPTR *mem = data;

	if	(mem->clean)
		mem->clean(mem->data);

	memfree(data);
}

static RefType mem_reftype = REFTYPE_INIT("data", NULL, mem_clean);

/*
The function |$1| creates a reference object with a dynamic allocated
memory field of size <size>. The data could be accesed by |rd_ptr|.
*/

void *rd_alloc (size_t size)
{
	REFPTR *mem = memalloc(sizeof *mem + size);
	mem->clean = NULL;
	mem->data = mem + 1;
	return rd_init(&mem_reftype, mem);
}

/*
The function |$1| creates a reference object with a foreign
pointer <ptr> and a cleanup function <clean>.
*/

void *rd_wrap (void *ptr, void (*clean) (void *data))
{
	REFPTR *mem = memalloc(sizeof *mem);
	mem->clean = clean;
	mem->data = ptr;
	return rd_init(&mem_reftype, mem);
}

/*
The function |$1| returns the data pointer of a reference object created
with |rd_alloc| or |rd_wrap|.
*/

void *rd_data (void *rd)
{
	return rd ? ((REFPTR *) rd)->data : NULL;
}

/*
$SeeAlso
refdata(3), memalloc(3).
*/
