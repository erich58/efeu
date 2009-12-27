/*
Informationsdatenbank

$Header	<EFEU/$1>

$Copyright (C) 1998 Erich Frühstück
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

#ifndef	_EFEU_Info_h
#define	_EFEU_Info_h	1

#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

/*	Info - Datenstruktur: 1. Element ist Ausgabefunktion
*/

typedef struct Info_s Info_t;

typedef void (*InfoPrint_t) (io_t *io, const Info_t *data);

typedef struct {
	admin_t admin;		/* Speicheradministration */
	InfoPrint_t pfunc;	/* Ausgabefunktion */
} InfoType_t;

struct Info_s {
	InfoType_t *type;
};

extern Info_t *Info_admin (Info_t *tg, const Info_t *src);
extern void Info_print (io_t *io, const Info_t *data);
extern void Info_subprint (io_t *io, const Info_t *data, const char *pfx);


/*
Der Datentype |$1| definiert eine Informationsstruktur mit einer
Zeichenkette als Informationsquelle.
*/

typedef struct {
	InfoType_t *type;
	char *fmt;
} InfoString_t;

extern InfoType_t InfoType_cstr;
extern InfoType_t InfoType_mstr;
Info_t *InfoString_create (char *label);

/*
Der Makro |$1| erlaubt die Initialisierung einer |$1_t| Informationsstruktur
mit der Zeichenkette <str>.
*/

#define	InfoString(str)	{ &InfoType_cstr, str }

/*
Der Datentype |$1| definiert eine Informationsstruktur mit einer
Ausgabefunktion als Informationsquelle.
*/

typedef struct {
	InfoType_t *type;
	void (*func) (io_t *io);
} InfoFunc_t;

extern InfoType_t InfoType_func;

/*
Der Makro |$1| erlaubt die Initialisierung einer |$1_t| Informationsstruktur
mit der Ausgabefunktion <func>.
*/

#define	InfoFunc(func)	{ &InfoType_func, func }

/*
Der Datentype |$1| definiert eine Informationsstruktur mit dem
aktuellen Sourcefile als Informationsquelle.
*/

typedef struct {
	InfoType_t *type;
	char *name;	/* Filename */
	int start;	/* Startzeile (wird nicht angezeigt) */
	int end;	/* Endezeile (wird nicht angezeigt) */
} FileLabel_t;

extern InfoType_t InfoType_FileLabel;

#define	Label(name)	Label_ ## name
#define	UseLabel(x)	extern FileLabel_t Label(x)
#define	GlobalLabel(x)	FileLabel_t Label(x) = \
	{ &InfoType_FileLabel, __FILE__, __LINE__,
#define	LocalLabel(x)	static GlobalLabel(x)
#define	EndLabel	__LINE__ };


/*	Informationsknoten
*/

typedef struct InfoNode_s InfoNode_t;
typedef void (*PrintInfo_t) (io_t *io, InfoNode_t *info);
typedef void (*LoadInfo_t) (InfoNode_t *info);

struct InfoNode_s {
	PrintInfo_t func;	/* Ausgabefunktion */
	char *name;		/* Knotenname */
	char *label;		/* Knotenbezeichnung */
	void *par;		/* Ausgabeparameter */
	InfoNode_t *prev;	/* Vorgängerknoten */
	LoadInfo_t load;	/* Ladefunktion */
	vecbuf_t *list;		/* Unterknoten */
};

extern char *InfoNameToken (char **ptr);

extern InfoNode_t *NewInfo (InfoNode_t *base, char *name);
extern InfoNode_t *GetInfo (InfoNode_t *base, const char *name);
extern InfoNode_t *AddInfo (InfoNode_t *base, const char *name,
	const char *label, PrintInfo_t func, void *par);

#define	INFO_ESC	'\\'	/* Fluchtsymbol in Infodatenbanken */
#define	INFO_SEP	'/'	/* Trennzeichen im Informationspfad */
#define	INFO_INPUT	'<'	/* Eingabefile */
#define	INFO_PIPE	'|'	/* Eingabepipeline */
#define	INFO_KEY	'@'	/* Kennung eines Informationspfades */


/*	Ausgabedefinitionen
*/

extern int InfoName (io_t *io, InfoNode_t *base, InfoNode_t *info);
extern void PrintInfo (io_t *io, InfoNode_t *base, const char *name);
extern void DumpInfo (io_t *io, InfoNode_t *base, const char *name);

extern char *InfoPath;

extern void SetInfoPath (const char *path);
extern void LoadInfo (InfoNode_t *base, const char *name);
extern void IOLoadInfo (InfoNode_t *base, io_t *io);


#endif	/* EFEU/Info.h */
