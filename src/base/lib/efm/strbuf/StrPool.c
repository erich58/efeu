/*
:*:string data pool
:de:Sammelbecken für Zeichenketten

$Header	<EFEU/$1>

$Copyright (C) 2007 Erich Frühstück
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <EFEU/StrPool.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>

#define	POOL_BSIZE	1000

#ifndef	MAP_FAILED
#define	MAP_FAILED	((void *) -1)
#endif

static char magic[8] = "StrPool";

static char *pool_ident (const void *ptr)
{
	const StrPool *pool = ptr;
	return msprintf("%zu/%zu + %zu/%zu",
		pool->cpos, pool->csize, pool->used, pool->msize);
}

static void pool_clean (void *ptr)
{
	StrPool_clean(ptr);
	memfree(ptr);
}

RefType StrPool_reftype = REFTYPE_INIT("StrPool", pool_ident, pool_clean);

void StrPool_init (StrPool *pool)
{
	pool->clean = NULL;
	pool->cdata = magic;
	pool->cpos = sizeof magic;
	pool->csize = sizeof magic;

	pool->mdata = NULL;
	pool->msize = 0;
	pool->used = 0;
}

void StrPool_clean (StrPool *pool)
{
	if	(pool->clean)
		pool->clean(pool->cdata, pool->csize);

	lfree(pool->mdata);
}

StrPool *StrPool_map (const char *path)
{
	StrPool *pool;
	struct stat buf;
	int fd;

	if	((fd = open(path, O_RDONLY)) == EOF)
		log_error(NULL, "$!: open: $1\n", "s", strerror(errno));

	if	(fstat(fd, &buf) == EOF)
		log_error(NULL, "$!: stat: $1\n", "s", strerror(errno));

	pool = memalloc(sizeof *pool);

	pool->clean = munmap;
	pool->csize = buf.st_size;
	pool->cpos = 1;
	pool->cdata = mmap(NULL, pool->csize, PROT_READ, MAP_SHARED, fd, 0);

	if	(pool->cdata == MAP_FAILED)
		log_error(NULL, "$!: mmap: $1\n", "s", strerror(errno));

	if	(pool->csize >= sizeof(magic) &&
			memcmp(pool->cdata, magic, sizeof magic) == 0)
	{
		pool->cpos = sizeof magic;
	}
	else	pool->cpos = 0;

	pool->mdata = NULL;
	pool->msize = 0;
	pool->used = 0;

	return rd_init(&StrPool_reftype, pool);
}

StrPool *NewStrPool (void)
{
	StrPool *pool = memalloc(sizeof *pool);
	StrPool_init(pool);
	return rd_init(&StrPool_reftype, pool);
}

static int str_clean (void *data, size_t dim)
{
	lfree(data);
	return 0;
}

StrPool *StrPool_alloc (size_t size)
{
	StrPool *pool = NewStrPool();
	pool->clean = str_clean;
	pool->cpos = sizeof magic;
	pool->csize = size + sizeof magic;
	pool->cdata = lmalloc(pool->csize);
	memcpy(pool->cdata, magic, sizeof magic);
	return pool;
}

size_t StrPool_read (StrPool *pool, size_t size, IO *in)
{
	if	(pool && in && pool->cpos + size <= pool->csize)
	{
		size_t n = io_read(in, pool->cdata + pool->cpos, size);
		pool->cpos += n;
		return n;
	}
	else	return 0;
}

size_t StrPool_write (StrPool *pool, IO *out)
{
	if	(pool)
	{
		size_t n = io_write(out, pool->cdata, pool->csize);
		n += io_write(out, pool->mdata, pool->used);
		return n;
	}
	else	return 0;
}

StrPool *StrPool_ext (void *data, size_t size,
	int (*clean) (void *data, size_t size))
{
	StrPool *pool = NewStrPool();
	pool->clean = clean;
	pool->cdata = data;
	pool->csize = size;
	pool->cpos = 0;
	return pool;
}

void StrPool_save (StrPool *pool, const char *path)
{
	int fd;

	if	((fd = creat(path, 0666)) < 0)
		log_error(NULL, "$!: creat: $1\n", "s", strerror(errno));

	write(fd, pool->cdata, pool->csize);
	write(fd, pool->mdata, pool->used);
	close(fd);
}

size_t StrPool_offset (StrPool *pool)
{
	return pool ? (pool->csize + pool->used) : 0;
}

void StrPool_expand (StrPool *pool, size_t s)
{
	size_t n = pool->msize ? pool->msize : POOL_BSIZE;
	pool->msize += n * ((s + n - 1) / n);
	pool->mdata = lrealloc(pool->mdata, pool->msize);
}

size_t StrPool_add (StrPool *pool, const char *str)
{
	if	(pool && str)
	{
		size_t s, n;
		
		s = strlen(str) + 1;

		if	(pool->used + s >= pool->msize)
			StrPool_expand(pool, s);

		n = pool->used;
		memcpy(pool->mdata + pool->used, str, s);
		pool->used += s;
		return pool->csize + n;
	}
	else	return 0;
}

size_t StrPool_xadd (StrPool *pool, const char *str)
{
	return StrPool_add(pool, str ? str : "");
}

size_t StrPool_align (StrPool *pool, size_t size)
{
	size = size * ((pool->used + size - 1) / size);

	if	(pool->msize < size)
	{
		pool->msize = size;
		pool->mdata = lrealloc(pool->mdata, pool->msize);
	}

	memset(pool->mdata + pool->used, 0, size - pool->used);
	pool->used = size;
	return size;
}

size_t StrPool_start (StrPool *pool)
{
	if	(pool)
		pool->cpos = 0;

	return StrPool_next(pool);
}

size_t StrPool_next (StrPool *pool)
{
	if	(pool && pool->cpos < pool->csize)
	{
		size_t n = pool->cpos;

		while (pool->cdata[pool->cpos])
		{
			pool->cpos++;

			if	(pool->cpos >= pool->csize)
				return n;
		}

		pool->cpos++;
		return n;
	}
	else	return 0;
}

char *StrPool_get (const StrPool *pool, size_t offset)
{
	if	(!pool || !offset)	return NULL;
	if	(offset < pool->csize)	return pool->cdata + offset;

	offset -= pool->csize;

	if	(offset < pool->msize)	return pool->mdata + offset;

	return NULL;
}

char *StrPool_mget (const StrPool *pool, size_t offset)
{
	return mstrcpy(StrPool_get(pool, offset));
}

size_t StrPool_copy (StrPool *tg, StrPool *src, size_t offset)
{
	if	(tg == src)
		return offset;

	return StrPool_add(tg, StrPool_get(src, offset));
}

int StrPool_cmp (StrPool *p1, size_t i1, StrPool *p2, size_t i2)
{
	if	(p1 == p2 && i1 == i2)
		return 0;

	return mstrcmp(StrPool_get(p1, i1), StrPool_get(p2, i1));
}

