/*
:*:file mapping
:de:Datei mapping

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
#include <EFEU/MapFile.h>
#include <EFEU/mstring.h>
#include <EFEU/Debug.h>

#ifndef	MAP_FAILED
#define	MAP_FAILED	((void *) -1)
#endif

static char *map_ident (const void *ptr)
{
	const MapFile *map = ptr;
	return msprintf("%s[%zu]", map->name, map->size);
}

static void map_clean (void *ptr)
{
	MapFile *map = ptr;
	munmap(map->data, map->size);
	memfree(map->name);
	memfree(map);
}

RefType MapFile_reftype = REFTYPE_INIT("MapFile", map_ident, map_clean);

MapFile *MapFile_open (const char *path)
{
	MapFile *map;
	void *data;
	struct stat buf;
	int fd;

	if	((fd = open(path, O_RDONLY)) == EOF)
		return NULL;

	if	(fstat(fd, &buf) == EOF)
	{
		close(fd);
		return NULL;
	}

	data = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	close(fd);

	if	(data == MAP_FAILED)
	{
		dbg_error(NULL, "$!: mmap: $1.\n", "s", strerror(errno));
		return NULL;
	}

	map = memalloc(sizeof *map);
	map->name = mstrcpy(path);
	map->data = data;
	map->size = buf.st_size;
	return rd_init(&MapFile_reftype, map);
}
