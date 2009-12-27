/*
Stichprobenauswahl

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

#include <EFEU/EDB.h>
#include <EFEU/EDBFilter.h>
#include <EFEU/konvobj.h>
#include <EFEU/Random.h>

typedef struct {
	REFVAR;
	EDB *base;
	Random *rand;
	CmpDef *cmp;
	double size;
	EfiObj *last;
	int choose;
} SAMPLE;

static void var_clean (void *data)
{
	SAMPLE *par = data;
	rd_deref(par->base);
	rd_deref(par->rand);
	rd_deref(par->cmp);
	UnrefObj(par->last);
	memfree(par);
}

static RefType sample_reftype = REFTYPE_INIT("EDB_SAMPLE", NULL, var_clean);

static int sample_std (EfiType *type, void *data, void *opaque_par)
{
	SAMPLE *par = opaque_par;

	while (edb_read(par->base))
	{
		if	(UniformRandom(par->rand) < par->size)
			return 1;
	}

	return 0;
}

static int sample_cmp (EfiType *type, void *data, void *opaque_par)
{
	SAMPLE *par = opaque_par;

	while (edb_read(par->base))
	{
		if	(cmp_data(par->cmp, par->last->data, data) != 0)
		{
			AssignData(type, par->last->data, data);
			par->choose = UniformRandom(par->rand) < par->size;
		}

		if	(par->choose)
			return 1;
	}

	return 0;
}

static EDB *fdef_sample (EDBFilter *filter, EDB *base,
	const char *opt, const char *arg)
{
	SAMPLE *par;
	EDB *edb;
	char *p;
	
	par = memalloc(sizeof *par);
	par->base = rd_refer(base);
	par->size = arg ? 0.01 * C_strtod(arg, NULL) : 1.;
	p = NULL;
	par->rand = str2Random(opt, &p);
	par->cmp = NULL;

	if	(p)
	{
		while (isspace(*p) || *p == ',' || *p == ';')
			p++;

		if	(*p)
			par->cmp = cmp_create(base->obj->type, p, NULL);
	}

	par->last = par->cmp ? NewObj(base->obj->type, NULL) : NULL;
	par->choose = 0;

	edb = edb_share(base);
	edb->read = par->cmp ? sample_cmp : sample_std;
	edb->ipar = rd_init(&sample_reftype, par);
	return edb;
}

EDBFilter EDBFilter_sample = EDB_FILTER(NULL,
	"sample", "[rng;cmp]=weight", fdef_sample, NULL,
	":*:random sample"
	":de:Zufallsstichprobe"
);
