/*	Datenbank sortieren, Eintrag suchen
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/vecbuf.h>
#include <EFEU/database.h>


/*	Vergleichsfunktion für Dateneinträge
*/

static Func_t *DB_cmpfunc = NULL;

static int DB_cmp(const void *a, const void *b)
{
	int n;

	CallFunc(&Type_int, &n, DB_cmpfunc, a, b);
	return n;
}


/*	Eintrag suchen/ersetzen/löschen
*/

void *DB_search(DataBase_t *db, void *data, int flag)
{
	Func_t *save;

	save = DB_cmpfunc;
	DB_cmpfunc = db->cmp;
	data = vb_search(&db->buf, data, db->cmp ? DB_cmp : NULL, flag);
	DB_cmpfunc = save;
	return data;
}


/*	Datenbank sortieren
*/

void DB_sort(DataBase_t *db)
{
	if	(db && db->cmp)
	{
		Func_t *save;

		save = DB_cmpfunc;
		DB_cmpfunc = db->cmp;
		qsort(db->buf.data, db->buf.used, db->type->size, DB_cmp);
		DB_cmpfunc = save;
	}
}
