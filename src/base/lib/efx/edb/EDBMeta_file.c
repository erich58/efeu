/*
Dateiliste zusammenhängen/mischen/sortieren
$Copyright (C) 2006 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDBMeta.h>

#define	M_CAT	0
#define	M_MERGE	1
#define	M_SORT	2

typedef struct CATPAR {
	REFVAR;
	EDB *sub;
	char *path;
	char **tab;
	size_t dim;
} CATPAR;

static void cat_clean (void *data)
{
	CATPAR *cat = data;
	int i;

	for (i = 0; i < cat->dim; i++)
		memfree(cat->tab[i]);

	rd_deref(cat->sub);
	memfree(cat->path);
	memfree(cat);
}

static RefType cat_reftype = REFTYPE_INIT("EDB_CAT", NULL, cat_clean);

static int cat_read (EfiType *type, void *data, void *par)
{
	CATPAR *cat = par;

	for (;;)
	{
		if	(cat->sub->read(type, data, cat->sub->ipar))
			return 1;

		edb_closein(cat->sub);
		
		if	(cat->dim)
		{
			edb_paste(cat->sub, edb_fopen(cat->path, cat->tab[0]));
			memfree(cat->tab[0]);
			cat->tab++;
			cat->dim--;
		}
		else	return 0;
	}

	return 0;
}


static char *get_next (char **flist)
{
	char *p = *flist;

	while (*p == 0)
		p++;

	*flist = p + strlen(p);
	return p;
}

static int get_mode (const char *arg, char **key)
{
	int mode = M_CAT;
	char *p;

	if	(arg)
	{
		if	(strncmp("merge", arg, 5) == 0)
		{
			mode = M_MERGE;
		}
		else if	(strncmp("sort", arg, 4) == 0)
		{
			mode = M_SORT;
		}
	}

	if	(mode)
	{
		p = strchr(arg, '=');
		*key = p ? mstrcpy(p + 1) : NULL;
	}
	else	*key = NULL;

	return mode;
}

void EDBMeta_file (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	char *flist;
	char *key;
	EfiType *type;
	EDB *edb;
	int i, n;
	int mode;

	mode = get_mode(arg, &key);
	flist = EDBMeta_par(meta, 1);

	if	(!flist || !flist[0])
	{
		memfree(key);
		return;
	}

	for (n = 0, i = 1; flist[i]; i++)
	{
		if	(flist[i] == '\n')
		{
			flist[i] = 0;

			if (flist[i-1]) n++;
		}
	}
	
	edb = edb_fopen(meta->path, get_next(&flist));

	if	(!meta->cur)
	{
		meta->cur = edb;
	}
	else if	(meta->cur->read)
	{
		meta->prev = edb_paste(meta->prev, meta->cur);
		meta->cur = edb;
	}
	else	meta->cur = edb_paste(meta->cur, edb);

	type = meta->cur->obj->type;

	if	(n < 1)
	{
		if	(mode == M_SORT)
			meta->cur = edb_sort(meta->cur,
				cmp_create(type, key, NULL));
	}
	else if	(mode)
	{
		EDB **tab = memalloc(n * sizeof *tab);
		CmpDef *cmp = cmp_create(type, key, NULL);
		int i;

		tab[0] = (mode == M_SORT) ?
			edb_sort(meta->cur, rd_refer(cmp)) : meta->cur;

		for (i = 1; i < n; i++)
		{
			edb = edb_fopen(meta->path, get_next(&flist));

			if	(edb->obj->type != type)
			{
				tab[i] = edb_paste(edb_create(LvalObj(NULL,
					type), NULL), edb);
			}
			else	tab[i] = edb;

			if	(mode == M_SORT)
				tab[i] = edb_sort(tab[i], rd_refer(cmp));
		}

		meta->cur = edb_merge(cmp, tab, n);
		memfree(tab);
	}
	else
	{
		CATPAR *cat;
		
		n--;
		cat = memalloc(sizeof *cat + n * sizeof cat->tab[0]);
		cat->path = mstrcpy(meta->path);
		cat->tab = (void *) (cat + 1);
		cat->dim = n;
		cat->sub = meta->cur;
		meta->cur = edb_create(RefObj(cat->sub->obj), meta->desc);
		meta->desc = NULL;
		meta->cur->read = cat_read;
		meta->cur->ipar = rd_init(&cat_reftype, cat);

		for (i = 0; i < n; i++)
			cat->tab[i] = mstrcpy(get_next(&flist));
	}

	memfree(key);
}

void EDBMeta_paste (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	int mode;
	char *key;
	EfiType *type;

	if	(!meta->cur)	return;

	mode = get_mode(arg, &key);
	type = meta->cur->obj->type;

	if	(!meta->prev)
	{
		if	(mode == M_SORT)
			meta->cur = edb_sort(meta->cur,
				cmp_create(type, key, NULL));
	}
	else if	(mode)
	{
		CmpDef *cmp;
		EDB *tab[2];

		cmp = cmp_create(type, key, NULL);

		tab[0] = (mode == M_SORT) ?
			edb_sort(meta->cur, rd_refer(cmp)) : meta->cur;

		if	(meta->prev->obj->type != type)
		{
			tab[1] = edb_paste(edb_create(LvalObj(NULL,
				type), NULL), meta->prev);
		}
		else	tab[1] = meta->prev;

		if	(mode == M_SORT)
			tab[1] = edb_sort(tab[1], rd_refer(cmp));

		meta->cur = edb_merge(cmp, tab, 2);
	}
	else	meta->cur = edb_paste(meta->prev, meta->cur);

	meta->prev = NULL;
	memfree(key);
}
