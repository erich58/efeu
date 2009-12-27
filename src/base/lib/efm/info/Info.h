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
	void (*func) (IO *io, InfoNode *info); /* Ausgabefunktion */
	char *name;		/* Knotenname */
	char *label;		/* Knotenbezeichnung */
	void *par;		/* Ausgabeparameter */
	InfoNode *prev;	/* Vorgängerknoten */
	void (*load) (InfoNode *info);	/* Ladefunktion */
	VecBuf *list;		/* Unterknoten */
};

extern char *InfoNameToken (char **ptr);

extern InfoNode *NewInfo (InfoNode *base, char *name);
extern InfoNode *GetInfo (InfoNode *base, const char *name);
extern InfoNode *AddInfo (InfoNode *base, const char *name,
	const char *label, void (*func) (IO *io, InfoNode *info), void *par);

#define	INFO_ESC	'\\'	/* Fluchtsymbol in Infodatenbanken */
#define	INFO_SEP	'/'	/* Trennzeichen im Informationspfad */
#define	INFO_INPUT	'<'	/* Eingabefile */
#define	INFO_PIPE	'|'	/* Eingabepipeline */
#define	INFO_KEY	'@'	/* Kennung eines Informationspfades */


/*	Ausgabedefinitionen
*/

extern int InfoName (IO *io, InfoNode *base, InfoNode *info);
extern void PrintInfo (IO *io, InfoNode *base, const char *name);
extern void DumpInfo (IO *io, InfoNode *base, const char *name);

extern char *InfoPath;

extern void SetInfoPath (const char *path);
extern void LoadInfo (InfoNode *base, const char *name);
extern void IOLoadInfo (InfoNode *base, IO *io);


#endif	/* EFEU/Info.h */
