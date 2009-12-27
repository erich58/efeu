/*	Datenbank
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_DATABASE_H
#define	EFEU_DATABASE_H	1

#include <EFEU/object.h>
#include <EFEU/vecbuf.h>
#include <EFEU/refdata.h>


/*	Datenbankstruktur
*/

typedef struct {
	REFVAR;		/* Referenzzähler */
	Type_t *type;	/* Datentype */
	Func_t *cmp;	/* Vergleichsfunktion */
	vecbuf_t buf;	/* Datenbuffer */
} DataBase_t;

extern reftype_t DB_reftype;

DataBase_t *DB_create (Type_t *type, size_t blk);

void DB_load (io_t *io, DataBase_t *db, Obj_t *expr);
void DB_xload (io_t *io, DataBase_t *db, Obj_t *expr);

void DB_save (io_t *io, DataBase_t *db, int mode, VirFunc_t *test, char *list);

void *DB_search (DataBase_t *db, void *data, int flag);
void DB_sort (DataBase_t *db);

extern Type_t Type_DB;

void SetupDataBase (void);

#endif	/* EFEU_DATABASE_H */
