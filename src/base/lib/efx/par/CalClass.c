/*
Kalenderklassifikation

$Copyright (C) 2009 Erich Frühstück
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
#include <EFEU/cmdsetup.h>
#include <EFEU/calendar.h>
#include <EFEU/EfiClass.h>
#include <limits.h>
#include <time.h>

typedef struct {
	REFVAR;
	int ug;
	int og;
	int (*get_cal) (const EfiObj *base);
} CalClassPar;

static void CalClass_free (void *data)
{
	memfree(data);
}

static RefType CalClass_reftype = REFTYPE_INIT("CalClass", NULL, CalClass_free);

static int CalClass_update (const EfiObj *base, void *par)
{
	if	(par)
	{
		CalClassPar *cpar = par;
		int val = cpar->get_cal(base);
		
		if	(val < cpar->ug)	return 0;
		if	(val > cpar->og)	return 0;

		return val - cpar->ug + 1;
	}
	else	return 1;
}

void CalClass (EfiClassArg *def, const EfiType *type,
	const char *opt, const char *arg, void *get)
{
	CalClassPar *par;
	int i, n;
	char *p;
	
	par = memalloc(sizeof *par);
	par->get_cal = get;
	par->ug = str2Calendar(arg, &p, 0);

	if	(p && *p == ':')
		par->og = str2Calendar(p + 1, &p, 1);
	else	par->og = str2Calendar(arg, &p, 1);

 	def->type = NewEnumType(NULL, 1);
	n = par->og - par->ug + 1;

	for (i = 0; i < n; i++)
 		AddEnumKey(def->type, Calendar2str(opt, par->ug + i),
				NULL, i + 1);

 	def->type = AddEnumType(def->type);
	def->update = CalClass_update;
	def->par = rd_init(&CalClass_reftype, par);
}
