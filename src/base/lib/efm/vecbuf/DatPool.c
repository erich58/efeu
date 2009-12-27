/*
:*:data pool
:de:Sammelbecken für Daten

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
#include <EFEU/DatPool.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>

#ifndef	MAP_FAILED
#define	MAP_FAILED	((void *) -1)
#endif

#define	POOL_BSIZE	100000

static char magic[8] = "DatPool";

#define	MSIZE	(sizeof magic)

struct DatPool {
	REFVAR;
	int (*clean) (void *data, size_t size);
	char *cdata;
	char *mdata;
	size_t cpos;
	size_t csize;
	size_t used;
	size_t msize;
	size_t bsize;
	size_t offset;
};

static char *pool_ident (const void *ptr)
{
	const DatPool *pool = ptr;
	return msprintf("%lu@%lu + %lu@%lu/%lu",
		(uint32_t) pool->cpos, (uint32_t) pool->csize,
		(uint32_t) pool->used, (uint32_t) pool->msize,
		(uint32_t) pool->bsize);
}

static void pool_clean (void *ptr)
{
	DatPool *pool = ptr;

	if	(pool->clean)
		pool->clean(pool->cdata, pool->csize);

	memfree(pool);
}

RefType DatPool_reftype = REFTYPE_INIT("DatPool", pool_ident, pool_clean);

static int munmap_magic (void *data, size_t size)
{
	return munmap((char *) data - MSIZE, size + MSIZE);
}

DatPool *DatPool_map (const char *path)
{
	DatPool *pool;
	struct stat buf;
	int fd;

	if	((fd = open(path, O_RDONLY)) == EOF)
		dbg_error(NULL, "$!: open: $1\n", "s", strerror(errno));

	if	(fstat(fd, &buf) == EOF)
		dbg_error(NULL, "$!: stat: $1\n", "s", strerror(errno));

	pool = memalloc(sizeof *pool);
	pool->cpos = 0;

	pool->clean = munmap;
	pool->csize = buf.st_size;
	pool->cdata = mmap(NULL, pool->csize, PROT_READ, MAP_SHARED, fd, 0);

	if	(pool->cdata == MAP_FAILED)
		dbg_error(NULL, "$!: mmap: $1\n", "s", strerror(errno));

	if	(pool->csize >= MSIZE && memcmp(pool->cdata, magic, MSIZE) == 0)
	{
		pool->clean = munmap_magic;
		pool->csize -= MSIZE;
		pool->cdata = (char *) pool->cdata + MSIZE;
	}

	pool->mdata = NULL;
	pool->msize = 0;
	pool->used = 0;
	pool->bsize = 0;

	return rd_init(&DatPool_reftype, pool);
}

DatPool *DatPool_dyn (size_t bsize)
{
	DatPool *pool;

	pool = memalloc(sizeof *pool);

	pool->clean = NULL;
	pool->cdata = NULL;
	pool->cpos = 0;
	pool->csize = 0;

	pool->mdata = NULL;
	pool->msize = 0;
	pool->used = 0;
	pool->bsize = bsize;

	return rd_init(&DatPool_reftype, pool);
}

size_t DatPool_add (DatPool *pool, const void *data, size_t size)
{
	if	(pool && data && size)
	{
		size_t n;
		
		if	(pool->used + size >= pool->msize)
		{
			if	(pool->bsize)	n = pool->bsize;
			else if	(pool->msize)	n = pool->msize;
			else			n = POOL_BSIZE;

			pool->msize += n * ((size + n - 1) / n);
			pool->mdata = lrealloc(pool->mdata, pool->msize);
		}

		n = pool->used;
		memcpy(pool->mdata + pool->used, data, size);
		pool->used += size;
		return pool->csize + n;
	}
	else	return 0;
}

void *DatPool_get (DatPool *pool, size_t offset)
{
	if	(!pool)
		return NULL;

	if	(offset < pool->csize)
		return pool->cdata + offset;

	offset -= pool->csize;

	if	(offset < pool->msize)
		return pool->mdata + offset;

	return NULL;
}

void DatPool_create (DatPool *pool, const char *path)
{
	int fd;

	if	((fd = creat(path, 0666)) < 0)
		dbg_error(NULL, "$!: creat: $1\n", "s", strerror(errno));

	write(fd, magic, MSIZE);
	write(fd, pool->cdata, pool->csize);
	write(fd, pool->mdata, pool->used);
	close(fd);
}
