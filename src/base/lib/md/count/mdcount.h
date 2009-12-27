/*
Auszählen von mehrdimensionalen Datenmatrizen

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Frühstück
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

#ifndef	EFEU_mdcount_h
#define	EFEU_mdcount_h	1

#include <EFEU/mdmat.h>
#include <EFEU/vecbuf.h>
#include <EFEU/label.h>
#include <EFEU/Info.h>

/*	Zählerdefinition
*/

typedef struct {
	char *name;
	char *type;
	char *desc;
	int (*set) (const void *data);
	void (*init) (void *data, size_t *idx, size_t dim);
	void (*add) (void *data);
} MdCntObj;

typedef struct {
	REFVAR;
	VecBuf tab;
} MdCntObjTab;

MdCntObjTab * MdCntObjTab_create (void);
MdCntObj *MdCntObj_get (MdCntObjTab *tab, const char *name);
void MdCntObj_add (MdCntObjTab *tab, MdCntObj *entry, size_t dim);
void MdCntObjInfo (InfoNode *info, MdCntObjTab *tab);


/*	Klassifikationsdefinitionen
*/

typedef struct MdClassStruct MdClass;

#define	MDCLASS_VAR \
	char *name; \
	char *desc; \
	size_t dim; \
	Label *label; \
	int (*classify) (MdClass *cdef, const void *ptr)

struct MdClassStruct {
	MDCLASS_VAR;
};

typedef struct {
	REFVAR;
	VecBuf tab;
} MdClassTab;

MdClassTab * MdClassTab_create (void);

/*
Der Makro $1 bestimmt den Index einer Klassifikationsdefinition
bei gegebenenParameterwerten.
*/

#define	 MdClassify(cdef, ptr)	\
	(((cdef) && (cdef)->classify) ? (cdef)->classify((cdef), ptr) : 0)

extern char *MdClassListFormat;
extern char *MdClassPrintHead;
extern char *MdClassPrintEntry;
extern char *MdClassPrintFoot;
extern int MdClassPrintLimit;

void MdClassList (IO *io, MdClass *cdef);
void MdClassPrint (IO *io, MdClass *cdef);
void MdShowClass (IO *io, MdClassTab *tab, const char *plist);
void MdClassInfo (InfoNode *info, MdClassTab *tab);

void MdClass_add (MdClassTab *class, MdClass *tab, size_t dim);

MdClass *md_subclass (MdClass *base, const char *def);

/*	Pointerklassen
*/

typedef struct {
	MDCLASS_VAR;
	void *ptr;
} MdPtrClass;

MdPtrClass *MdPtrClass_copy (MdPtrClass *tab, size_t dim,
	const char *ext, void *ptr);
void MdPtrClass_add (MdClassTab *class, MdPtrClass *tab, size_t dim);
void MdPtrClass_xadd (MdClassTab *class, MdPtrClass *tab, size_t dim,
	const char *ext, void *ptr);
MdClass *MdClass_get (MdClassTab *tab, const char *name);


/*	Zählgruppe
*/

typedef struct MdCntGrpStruct {
	struct MdCntGrpStruct *next;	/* Nächste Definition */
	MdClass *cdef;		/* Klassifikationsdefinition */
	int flag;		/* Selektionsflag */
	int idx;		/* Selektionsindex */
} MdCntGrp;


/*	Zähltabellen
*/

mdaxis *md_classaxis (const char *name, ...);
mdaxis *md_ctabaxis (IO *io, MdClassTab *stab);
mdmat *md_ctab (const char *titel, const char *def,
	MdClassTab *gtab, MdCntObj *counter);
mdmat *md_ioctab (const char *titel, IO *io,
	MdClassTab *gtab, MdCntObj *counter);

void md_ctabinit (mdmat *md, MdCntObj *counter);
void md_count (mdmat *tab, const void *data);
void md_showcnt (IO *io, MdCntObjTab *tab);

extern MdCntObj *stdcount;	/* Standardzähler mit long - Werten */

#endif	/* EFEU/mdcount.h */
