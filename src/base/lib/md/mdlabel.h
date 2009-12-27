/*
Labelgenerierung

$Header <EFEU/$1>

$Copyright (C) 1997 Erich Frühstück
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

#ifndef EFEU_mdlabel_h
#define	EFEU_mdlabel_h	1

#include <EFEU/mdmat.h>

/*	Labelstruktur
*/

typedef struct MdLabelStruct MdLabel;


struct MdLabelStruct {
	MdLabel *next;
	char *name;	/* Name der Achse */
	size_t len;	/* Länge des Bezeichners */
	size_t dim;	/* Zahl der Muster */
	char **list;	/* Liste mit Muster */
	VecBuf idxtab;	/* Tabelle mit Bezeichnern */
};


MdLabel *new_label (void);
void del_label (MdLabel *label);
MdLabel *set_label (const char *def);
MdLabel *init_label (const char *fmt, const char *def);

int save_label (IO *tmp, MdLabel *label, char *p);
mdaxis *label2axis (MdLabel *label);


#define	ICTRL_HEAD	1	/* Wechsel in Headermodus */
#define	ICTRL_TEXT	2	/* Wechsel in Textmode */
#define	ICTRL_NUM	3	/* Wechsel in numerischen Mode */
#define	ICTRL_IGNORE	4	/* Ignorieren von Zeichen im num. Mode */

IO *rfilter (IO *io);


#endif	/* EFEU/mdlabel.h */
