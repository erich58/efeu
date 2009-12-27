/*	Datenbank oeffnen
	(c) 1992 Erich Fruehstueck
	A-1090 Wien, Waehringer Strasse 64/6
*/


#include <EFEU/ftools.h>
#include <EFEU/efmain.h>
#include "agf.h"

FILE *popen (const char *cmd, const char *type);
int pclose (FILE *stream);

static FILE *db_file = NULL;
static ushort_t *db_data = NULL;
static size_t db_size = 0;

static int (*do_close) (FILE *file);


void open_db(def)

DBDEF *def;

{
	char *p;

	if	((p = fsearch(NULL, NULL, def->file, NULL)) != NULL)
	{
		db_file = fileopen(p, "rb");
		do_close = fileclose;
		FREE(p);
	}
	else if	((p = fsearch(NULL, NULL, def->file, "gz")) != NULL)
	{
		reg_set(1, p);
		p = parsub("zcat $1");
		db_file = popen(p, "rb");
		do_close = pclose;
		FREE(p);
	}

	db_data = ALLOC(def->size, ushort_t);
	db_size = def->size * sizeof(ushort_t);
}


void close_db()

{
	do_close(db_file);
	FREE(db_data);
	db_file = NULL;
	db_data = NULL;
}


ushort_t *get_db()

{
	if	(db_file == NULL || db_data == NULL)
	{
		return NULL;
	}
	else if	(fread(db_data, db_size, 1, db_file))
	{
#if	REVBYTEORDER
		uchar_t *data, c;
		int i;

		data = (uchar_t *) db_data;

		for (i = 0; i < db_size; i += 2)
		{
			c = data[i];
			data[i] = data[i+1];
			data[i+1] = c;
		}
#endif
		return db_data;
	}
	else	return NULL;
}
