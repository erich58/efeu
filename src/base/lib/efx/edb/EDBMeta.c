/*
EDB-Metadatenfile
$Copyright (C) 2006 Erich Frühstück
A-3423 St.Andrä/Wördern, Wildenhaggasse 38
*/

#include <EFEU/EDBMeta.h>
#include <EFEU/konvobj.h>

#define	FTAB_BLK	500

void EDBMeta_init (EDBMeta *meta, IO *base, IO *ctrl)
{
	meta->base = base;
	meta->ctrl = ctrl;
	meta->path = NULL;
	meta->desc = NULL;
	meta->prev = NULL;
	meta->cur = NULL;
	meta->list = NULL;
	sb_init(&meta->buf, 0);
}

void EDBMeta_clean (EDBMeta *meta)
{
	rd_deref(meta->base);
	rd_deref(meta->ctrl);
	memfree(meta->path);
	memfree(meta->desc);
	rd_deref(meta->prev);
	rd_deref(meta->cur);
	meta->base = NULL;
	meta->ctrl = NULL;
	meta->path = NULL;
	meta->desc = NULL;
	meta->prev = NULL;
	meta->cur = NULL;
	meta->list = NULL;
	sb_free(&meta->buf);
}


EDB *EDBMeta_edb (EDBMeta *meta)
{
	EDB *edb;

	edb = edb_paste(rd_refer(meta->prev), rd_refer(meta->cur));
	EDBMeta_clean(meta);
	return edb;
}

int EDBMeta_list (EDBMeta *meta)
{
	char *p = EDBMeta_par(meta, 1);

	if	(p && p[0])
	{
		int i, n;

		meta->list = p;

		for (n = 0, i = 1; p[i]; i++)
		{
			if	(p[i] == '\n')
			{
				p[i] = 0;

				if (p[i-1]) n++;
			}
		}

		if (p[i-1]) n++;

		return n;
	}

	return 0;
}

char *EDBMeta_next (EDBMeta *meta)
{
	char *p = meta->list;

	while (*p == 0)
		p++;

	meta->list = p + strlen(p);
	return p;
}
