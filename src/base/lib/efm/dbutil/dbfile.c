/*	Datenbankfiles
	(c) 1995 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/dbutil.h>
#include <EFEU/procenv.h>


/*	Dummy - Ein/Ausgabefunktionen
*/

static size_t no_read (void *data, size_t size, size_t nitems, FILE *file)
{
	fileerror(file, MSG_DB, 1, 0);
	return 0;
}

static size_t no_write (const void *data, size_t size, size_t nitems, FILE *file)
{
	fileerror(file, MSG_DB, 2, 0);
	return 0;
}


/*	Datenfile öffnen
*/

DBFILE *db_xopen(const char *name, const char *mode, size_t size, db_read_t read, db_write_t write)
{
	DBFILE *db;

	db = memalloc(sizeof(DBFILE) + size);
	db->file = fileopen(name, mode);
	db->read = read ? read : no_read;
	db->write = write ? write : no_write;
	db->reclen = size;
	db->buf = NULL;
	return db;
}


DBFILE *db_open(const char *name, const char *mode, size_t size)
{
	return db_xopen(name, mode, size, fread, fwrite);
}


/*	Datenfile schließen
*/

void db_close(DBFILE *db)
{
	if	(db)
	{
		fileclose(db->file);
		memfree(db);
	}
}


/*	Datensätze lesen
*/

void *db_get(DBFILE *db)
{
	db->buf = (db->read(db + 1, db->reclen, 1, db->file) ? (db + 1) : NULL);
	return db->buf;
}

void *db_last(DBFILE *db)
{
	return db->buf;
}


size_t db_read(void *data, size_t dim, DBFILE *db)
{
	return db->read(data, db->reclen, dim, db->file);
}


/*	Datensätze ausgeben
*/

int db_put(const void *data, DBFILE *db)
{
	if	(db->write(data, db->reclen, 1, db->file) != 1)
	{
		fileerror(db->file, MSG_DB, 3, 0);
		return 0;
	}

	return 1;
}


size_t db_write(const void *data, size_t dim, DBFILE *db)
{
	if	(db->write(data, db->reclen, dim, db->file) != dim)
	{
		fileerror(db->file, MSG_DB, 3, 0);
		return 0;
	}

	return dim;
}


int db_uniq(const void *data, DBFILE *db)
{
	if	(db->buf && memcmp(db->buf, data, db->reclen) == 0)
		return 0;

	db->buf = (db + 1);
	memcpy(db->buf, data, db->reclen);
	return db_write(data, 1, db);
}
