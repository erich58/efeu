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
	EfiObj *obj;
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

	rd_deref(cat->obj);
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
		if	(edb_read(cat->sub))
			return 1;

		rd_deref(cat->sub);
		
		if	(cat->dim)
		{
			cat->sub = edb_conv(edb_fopen(cat->path,
				cat->tab[0]), type);
			cat->obj->data = cat->sub->obj->data;
			memfree(cat->tab[0]);
			cat->tab++;
			cat->dim--;
		}
		else
		{
			cat->obj->data = cat->obj + 1;
			cat->sub = NULL;
			break;
		}
	}

	return 0;
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
	char *key;
	EfiType *type;
	EDB *edb;
	int i, n;
	int mode;

	mode = get_mode(arg, &key);

	if	(!(n = EDBMeta_list(meta)))
	{
		memfree(key);
		return;
	}

	edb = edb_fopen(meta->path, EDBMeta_next(meta));

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

	if	(n <= 1)
	{
		if	(mode == M_SORT)
			meta->cur = edb_sort(meta->cur,
				key ? cmp_create(type, key, NULL) : NULL);
	}
	else if	(mode)
	{
		EDB **tab = memalloc(n * sizeof *tab);
		CmpDef *cmp = key ? cmp_create(type, key, NULL) : NULL;
		int i;

		tab[0] = (mode == M_SORT) ?
			edb_sort(meta->cur, rd_refer(cmp)) : meta->cur;

		for (i = 1; i < n; i++)
		{
			edb = edb_fopen(meta->path, EDBMeta_next(meta));

			if	(edb->obj->type != type)
			{
				tab[i] = edb_paste(edb_create(type), edb);
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
		cat->obj = LvalObj(NULL, cat->sub->obj->type);
		cat->obj->data = cat->sub->obj->data;
		meta->cur = edb_alloc(RefObj(cat->obj), meta->desc);
		meta->desc = NULL;
		meta->cur->read = cat_read;
		meta->cur->ipar = rd_init(&cat_reftype, cat);

		for (i = 0; i < n; i++)
			cat->tab[i] = mstrcpy(EDBMeta_next(meta));
	}

	memfree(key);
}


void EDBMeta_paste (EDBMetaDef *def, EDBMeta *meta, const char *arg)
{
	int mode;
	char *key;

	if	(!meta->cur)	return;

	mode = get_mode(arg, &key);

	if	(!meta->prev)
	{
		if	(mode == M_SORT)
			meta->cur = edb_sort(meta->cur,
				cmp_create(meta->cur->obj->type, key, NULL));
	}
	else if	(mode)
	{
		CmpDef *cmp;
		EDB *tab[2];
		EfiType *type;

		type = meta->prev->obj->type;
		cmp = key ? cmp_create(type, key, NULL) : NULL;

		tab[0] = (mode == M_SORT) ?
			edb_sort(meta->prev, rd_refer(cmp)) : meta->prev;

		if	(meta->cur->obj->type != type)
		{
			tab[1] = edb_paste(edb_create(type), meta->cur);
		}
		else	tab[1] = meta->cur;

		if	(mode == M_SORT)
			tab[1] = edb_sort(tab[1], rd_refer(cmp));

		meta->cur = edb_merge(cmp, tab, 2);
	}
	else	meta->cur = edb_paste(meta->prev, meta->cur);

	meta->prev = NULL;
	memfree(key);
}
