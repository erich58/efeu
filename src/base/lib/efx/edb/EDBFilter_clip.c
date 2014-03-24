/*	clip - filter

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

#include <EFEU/EDB.h>
#include <EFEU/EDBFilter.h>
#include <ctype.h>

typedef struct {
	REFVAR;
	EDB *base;
	size_t count;
} CLIP;

static void clip_clean (void *data)
{
	CLIP *clip = data;
	rd_deref(clip->base);
	memfree(clip);
}

static RefType clip_reftype = REFTYPE_INIT("EDB_CLIP", NULL, clip_clean);

static void *clip_alloc (EDB *edb, unsigned count)
{
	CLIP *clip = memalloc(sizeof *clip);
	clip->base = edb;
	clip->count = count;
	return rd_init(&clip_reftype, clip);
}

static int clip_read (EfiType *type, void *data, void *par)
{
	CLIP *clip = par;

	if	(clip->count && edb_read(clip->base))
	{
		clip->count--;
		return 1;
	}

	return 0;
}

static EDB *fdef_clip (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	EDB *edb;
	CLIP *clip;
	unsigned skip;
	unsigned count;
	
	if	(!arg)	return base;

	skip = strtoul(arg, (char **) &arg, 0);
	count = (arg && *arg == ',') ? strtoul(arg + 1, NULL, 0) : 0;
	clip = clip_alloc(base, skip);

	while (clip_read(base->obj->type, base->obj->data, clip))
		;

	if	(count)
	{
		clip->count = count;
		edb = edb_share(base);
		edb->read = clip_read;
		edb->ipar = clip;
		return edb;
	}

	rd_refer(base);
	rd_deref(clip);
	return base;
}

EDBFilter EDBFilter_clip = EDB_FILTER(NULL,
	"clip", "=skip,count", fdef_clip, NULL,
	":*:clips out a part of the database"
	":de:Datenbank auf Bereich einschränken"
);
