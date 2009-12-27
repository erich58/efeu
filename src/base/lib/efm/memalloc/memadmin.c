/*	Speicherplatzadministration
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6
*/


#include <EFEU/memalloc.h>
#include <EFEU/vecbuf.h>

void memclean (void **ptr)
{
	if	(ptr && *ptr)
	{
		memfree(*ptr);
		*ptr = NULL;
	}
}

void *memadmin (void *tg, const void *src, size_t size)
{
	void *rval;

	if	(tg && !src)
	{
		memfree(tg);
		return NULL;
	}

	rval = tg ? tg : memalloc(size);

	if	(src && src != tg)
		memcpy(rval, src, size);
	else if	(tg)
		memset(rval, 0, size);

	return rval;
}

void *admin_data (alloctab_t *tab, void *tg, const void *src)
{
	void *rval;

	if	(tg && !src)
	{
		del_data(tab, tg);
		return NULL;
	}

	if	(tab == NULL)	return NULL;

	rval = tg ? tg : new_data(tab);

	if	(src && src != tg)
		memcpy(rval, src, tab->elsize);
	else if	(tg)
		memset(rval, 0, tab->elsize);

	return rval;
}
