/*	Wert suchen/einfügen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>

#define	BLOCKSIZE	64	/* Vorgabe für Blockgröße */

static int ptr_cmp(const void *a, const void *b)
{
	if	(a < b)	return -1;
	else if	(a > b)	return 1;
	else		return 0;
}


/*	Suche nach Eintrag
*/

void *xsearch(xtab_t *xt, void *key, int flag)
{
	comp_t comp;
	int i, i1, i2, v;

	if	(xt == NULL)	return NULL;

	comp = (xt->comp ? xt->comp : ptr_cmp);

/*	Suche nach dem Eintrag
*/
	i1 = 0;
	i2 = xt->dim;

	while (i1 != i2)
	{
		i = (i1 + i2) / 2;
		v = (*comp)(key, xt->tab[i]);

		if	(v > 0)
		{
			i1 = i + 1;
		}	
		else if	(v < 0)
		{
			i2 = i;
		}
		else
		{
			void *ptr;

			ptr = xt->tab[i];

			if	(flag == XS_DELETE)
			{
				for (xt->dim--; i < xt->dim; i++)
					xt->tab[i] = xt->tab[i+1];
			}
			else if	(flag == XS_REPLACE)
			{
				xt->tab[i] = key;
			}

			return ptr;
		}
	}

/*	Nicht gefunden: Falls Flag verschieden von XS_FIND und XS_DELETE
	ist wird der Eintrag in die Tabelle eingefügt.
*/
	if	(flag == XS_FIND || flag == XS_DELETE)
	{
		return NULL;
	}

/*	Genügend Speicherplatz für neues Element vorhanden
*/
	if	(xt->dim < xt->tsize)
	{
		for (i = xt->dim; i > i2; i--)
			xt->tab[i] = xt->tab[i-1];

		xt->tab[i2] = key;
		xt->dim++;
	}

/*	Tabelle umkopieren
*/
	else
	{
		void **ptr;

		xt->tsize += (xt->bsize > 0 ? xt->bsize : BLOCKSIZE);
		ptr = (void **) lmalloc((ulong_t) (xt->tsize)
			* sizeof(void *));

		for (i = 0; i < i2; i++)
			ptr[i] = xt->tab[i];

		ptr[i2] = key;

		for (i = i2; i < xt->dim; i++)
			ptr[i + 1] = xt->tab[i];

		xt->dim++;
		lfree(xt->tab);
		xt->tab = ptr;
	}

	return (flag == XS_REPLACE ? NULL : key);
}
