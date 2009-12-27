/*
Daten einlesen
*/

#include <EFEU/EDB.h>
#include <EFEU/ioctrl.h>
#include <EFEU/printobj.h>
#include <EFEU/DBData.h>
#include <ctype.h>

typedef struct {
	REFVAR;
	int (*read) (EfiType *type, void *data, void *par);
	void *par;
	int (*next_read) (EfiType *type, void *data, void *par);
	void *next_par;
} PASTE;

static void paste_clean (void *data)
{
	PASTE *paste = data;
	rd_deref(paste->par);
	rd_deref(paste->next_par);
	memfree(paste);
}

static RefType paste_reftype = REFTYPE_INIT("EDB_PASTE", NULL, paste_clean);

static int paste_read (EfiType *type, void *data, void *par)
{
	PASTE *paste = par;
	int rval;

	while (paste->read)
	{
		if	((rval = paste->read(type, data, paste->par)))
			return rval;

		rd_deref(paste->par);
		paste->read = paste->next_read;
		paste->par = paste->next_par;
		paste->next_read = NULL;
		paste->next_par = NULL;
	}

	return 0;
}

void edb_input (EDB *edb, int (*read) (EfiType *, void *, void *), void *ipar)
{
	if	(edb->read)
	{
		PASTE *paste = memalloc(sizeof *paste);
		paste->read = edb->read;
		paste->par = edb->ipar;
		paste->next_read = read;
		paste->next_par = ipar;
		edb->read = paste_read;
		edb->ipar = rd_init(&paste_reftype, paste);
	}
	else
	{
		edb->read = read;
		edb->ipar = ipar;
	}
}
