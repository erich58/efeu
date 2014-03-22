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

#ifndef	EFEU_Info_h
#define	EFEU_Info_h	1

#include <EFEU/vecbuf.h>
#include <EFEU/io.h>

/*	Informationsknoten
*/

typedef struct InfoNodeStruct InfoNode;

struct InfoNodeStruct {
	REFVAR;			/* Referenzvariablen */
	char *name;		/* Knotenname */
	char *label;		/* Knotenbezeichnung */
	InfoNode *prev;		/* Vorgängerknoten */
	VecBuf *list;		/* Unterknoten */
	void (*setup) (InfoNode *info);	/* Ladefunktion */
	void (*func) (IO *io, InfoNode *info); /* Ausgabefunktion */
	void *par;		/* Ausgabeparameter */
};

extern RefType Info_reftype;

#define	STD_INFO(name,label)	\
{ REFDATA(&Info_reftype), name, label, NULL, NULL, NULL, NULL, NULL }

char *InfoNameToken (char **ptr);

void SetupInfo (InfoNode *node);
InfoNode *NewInfo (InfoNode *base, char *name);
InfoNode *GetInfo (InfoNode *base, const char *name);
InfoNode *AddInfo (InfoNode *base, const char *name,
	const char *label, void (*func) (IO *io, InfoNode *info), void *par);

#define	INFO_ESC	'\\'	/* Fluchtsymbol in Infodatenbanken */
#define	INFO_SEP	'/'	/* Trennzeichen im Informationspfad */
#define	INFO_INPUT	'<'	/* Eingabefile */
#define	INFO_PIPE	'|'	/* Eingabepipeline */
#define	INFO_KEY	'@'	/* Kennung eines Informationspfades */


/*	Ausgabedefinitionen
*/

int InfoName (IO *io, InfoNode *base, InfoNode *info);
void PrintInfo (IO *io, InfoNode *node);
void DumpInfo (IO *io, const char *name, int flag);
void BrowseInfo (const char *name);

extern void (*InfoBrowser) (const char *name);
extern void (*XInfoBrowser) (const char *name);
extern void StdInfoBrowser (const char *name);

extern char *InfoPath;

void SetInfoPath (const char *path);
void LoadInfo (InfoNode *base, const char *name);
void IOLoadInfo (InfoNode *base, IO *io);

#endif	/* EFEU/Info.h */
