/*	Datenbankdatentypen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

$Header	<EFEU/$1>
*/

#ifndef _EFEU_DBType_h
#define _EFEU_DBType_h	1

#include <EFEU/mstring.h>
#include <EFEU/data.h>
#include <EFEU/io.h>

typedef size_t (*dbiofunc_t) (io_t *io, void *data, size_t dim);
typedef void (*dbreorg_t) (void *data, size_t dim);

typedef struct {
	char *name;		/* Bezeichnung */
	char *label;		/* Beschreibung */
	size_t recl;		/* Datensatzlänge */
	size_t size;		/* Speicherbedarf */
	dbiofunc_t read;	/* Datensatz einlesen */
	dbiofunc_t write;	/* Datensatz ausgeben */
	dbreorg_t alloc;	/* Teilspeicher generieren */
	dbreorg_t clear;	/* Datensatz löschen */
	dbreorg_t sync;		/* Teilvektoren synchronisieren */
} DBType_t; 

void *NewDBData (DBType_t *dbd, size_t n);
void DelDBData (DBType_t *dbd, void *data, size_t n);

size_t ReadDBData (DBType_t *dbd, io_t *io, void *data, size_t n);
size_t WriteDBData (DBType_t *dbd, io_t *io, void *data, size_t n);
void AllocDBData (DBType_t *dbd, void *data, size_t n);
void ClearDBData (DBType_t *dbd, void *data, size_t n);
void SyncDBData (DBType_t *dbd, void *data, size_t n);

#endif	/* EFEU/DBType_h */
