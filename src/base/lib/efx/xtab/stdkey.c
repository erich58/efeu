/*	Standardsuchtabellen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>

nkey_t nkey_buf = { 0 };
skey_t skey_buf = { NULL };

typedef struct {
	int num;
	const void *ptr;
} NENTRY;


typedef struct {
	const char *name;
	const void *ptr;
} SENTRY;


static ALLOCTAB(skeytab, 32, sizeof(SENTRY));
static ALLOCTAB(nkeytab, 32, sizeof(NENTRY));

#define	NKEY(x)	(((const nkey_t *) (x))->num)
#define	SKEY(x)	(((const skey_t *) (x))->name)


/*	Vergleichsfunktion
*/

int skey_cmp(const void *a, const void *b)
{
	return mstrcmp(SKEY(a), SKEY(b));
}


/*	Pointer abfragen
*/

void *skey_get(xtab_t *tab, const char *name, const void *def)
{
	SENTRY *entry;

	entry = (SENTRY *) skey_find(tab, name);
	return (void *) (entry && entry->ptr ? entry->ptr : def);
}


/*	Wert setzen
*/

void *skey_set(xtab_t *tab, const char *name, const void *ptr)
{
	SENTRY *x, *y;

	if	(tab == NULL)	return NULL;

	x = (SENTRY *) new_data(&skeytab);
	x->name = mstrcpy(name);
	x->ptr = ptr;
	y = (SENTRY *) xsearch(tab, (void *) x, XS_ENTER);

	if	(y != x)
	{
		ptr = y->ptr;
		y->ptr = x->ptr;
		FREE(x->name);
		del_data(&skeytab, x);
		return (void *) ptr;
	}
	else	return NULL;
}


/*	Vergleichsfunktion
*/

int nkey_cmp(const void *a, const void *b)
{
	if	(NKEY(a) < NKEY(b))	return -1;
	else if	(NKEY(a) > NKEY(b))	return 1;
	else				return 0;
}


/*	Wert abfragen
*/

void *nkey_get(xtab_t *tab, int num, const void *def)
{
	NENTRY *entry;

	entry = (NENTRY *) nkey_find(tab, num);
	return (void *) (entry && entry->ptr ? entry->ptr : def);
}


/*	Wert setzen
*/

void *nkey_set(xtab_t *tab, int num, const void *ptr)
{
	NENTRY *x, *y;

	if	(tab == NULL)	return NULL;

	x = (NENTRY *) new_data(&nkeytab);
	x->num = num;
	x->ptr = ptr;
	y = (NENTRY *) xsearch(tab, (void *) x, XS_ENTER);

	if	(y != x)
	{
		ptr = y->ptr;
		y->ptr = x->ptr;
		del_data(&nkeytab, x);
		return (void *) ptr;
	}
	else	return NULL;
}
