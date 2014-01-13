/*
Arbeiten mit Datenbankdaten

$Header	<EFEU/$1>

$Copyright (C) 1995 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef EFEU_DBData_h
#define EFEU_DBData_h	1

#include <EFEU/io.h>
#include <EFEU/dbutil.h>
#include <EFEU/refdata.h>

typedef struct {
	unsigned recl;		/* Satzlänge */
	unsigned dim;		/* Felddimension */
	unsigned char *buf;	/* Datenbuffer */
	char **tab;		/* Feldvektor */
	unsigned buf_size;	/* Buffergröße */
	unsigned tab_size;	/* Tabellengröße */
	short ebcdic;		/* Flag für EBCDIC-Daten */
	short fixed;		/* Flag für Fixe Satzlänge */
} DBData;

#define	DBDATA(name)	DBData name = { 0, 0, NULL, NULL, 0, 0 }

DBData *DBData_ebcdic (DBData *db, IO *io, int recl);
DBData *DBData_text (DBData *db, IO *io, const char *delim);
DBData *DBData_qtext (DBData *db, IO *io, const char *delim);

void DBData_conv (DBData *db);
void DBData_sync (DBData *db, int recl, int c);
int DBData_split (DBData *db, const char *delim);

int DBData_check (DBData *data, int pos, int len);
unsigned char *DBData_ptr (DBData *data, int pos, int len);
int DBData_isblank (DBData *data, int pos, int len);
int DBData_isdigit (DBData *data, int pos, int len);
unsigned DBData_base37 (DBData *data, int pos, int len);
unsigned DBData_uint (DBData *data, int pos, int len);
char *DBData_str (DBData *data, int pos, int len);
char *DBData_xstr (DBData *data, int pos, int len);
int DBData_char (DBData *data, int pos, int len);
unsigned DBData_packed (DBData *data, int pos, int len);
unsigned DBData_bcdval (DBData *data, int pos, int len);
unsigned DBData_binary (DBData *data, int pos, int len);

char *DBData_field (DBData *data, int idx);
char *DBData_xfield (DBData *data, int idx);
int DBData_date (DBData *data, int pos, int flag);

#define	DBData_field_unsigned(db,idx,base)	\
	strtoul(DBData_field(db, idx), NULL, base)

#define	DBData_field_uint64(db,idx,base)	\
	mstr2uint64(DBData_field(db, idx), NULL, base)

#define	DBData_xfield_unsigned(db,idx,base)	\
	strtoul(DBData_xfield(db, idx), NULL, base)

#define	DBData_xfield_uint64(db,idx,base)	\
	mstr2uint64(DBData_xfield(db, idx), NULL, base)

double DBData_field_double (DBData *data, int pos);

typedef struct {
	REFVAR;
	IO *io;
	DBData data;
	int mode;
	int save;	/* Flag für Zurückschreiben */
	size_t recl;
	char *delim;
} DBFile;

#define	DBFILE_TEXT	0
#define	DBFILE_EBCDIC	1
#define	DBFILE_CONV	2
#define	DBFILE_QTEXT	3

extern RefType DBFile_reftype;

DBFile *DBFile_open (IO *io, int mode, size_t recl, const char *delim);
int DBFile_next (DBFile *file);
void DBFile_show (DBFile *file, IO *io);
void DBFile_unread (DBFile *file);

#endif	/* EFEU/DBData.h */
