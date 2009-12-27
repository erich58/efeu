/*
Ausz�hlen von mehrdimensionalen Datenmatrizen

$Header <EFEU/$1>

$Copyright (C) 1994 Erich Fr�hst�ck
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

/*	Z�hlerdefinition
*/

typedef struct MdCountStruct MdCount;

#define	MDCOUNT_VAR \
	char *name; \
	char *type; \
	char *desc; \
	int (*set) (MdCount *cnt, void *buf, const void *data); \
	void (*init) (MdCount *cnt, void *data, size_t *idx, size_t dim); \
	void (*add) (MdCount *cnt, void *data, void *buf)

struct MdCountStruct {
	MDCOUNT_VAR;
};

typedef struct MdCountListStruct {
	REFVAR;
	MdCount *cnt;
	EfiType *type;
	size_t offset;
	int flag;
	void *data;
	struct MdCountListStruct *next;
} MdCountList;

MdCountList *MdCountList_create (MdCount *cnt);
MdCountList *MdCountList_add (MdCountList *list, MdCount *cnt);

typedef struct {
	REFVAR;
	VecBuf vtab;	/* Variablentabelle */
	VecBuf ctab;	/* Klassifikationstabelle */
} MdCountTab;
	
MdCountTab *MdCountTab_create (void);


MdCount *MdCount_get (MdCountTab *tab, const char *name);
MdCountList *MdCountList_get (MdCountTab *tab, const char *name);
void MdCount_add (MdCountTab *tab, MdCount *entry, size_t dim);
void MdCountInfo (InfoNode *info, MdCountTab *tab);

extern EfiType Type_cotab;
extern EfiType Type_cntobj;

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
void MdShowClass (IO *io, MdCountTab *tab, const char *plist);
void MdClassInfo (InfoNode *info, MdCountTab *tab);

void MdClass_add (MdCountTab *class, MdClass *tab, size_t dim);
MdClass *MdClass_get (MdCountTab *tab, const char *name);

MdClass *md_subclass (MdClass *base, const char *def);

/*	Pointerz�hler und Pointerklassen
*/

typedef struct {
	MDCOUNT_VAR;
	void *ptr;
} MdPtrCount;

MdPtrCount *MdPtrCount_copy (MdPtrCount *tab, size_t dim,
	const char *ext, void *ptr);
void MdPtrCount_add (MdCountTab *tab, MdPtrCount *entry, size_t dim);
void MdPtrCount_xadd (MdCountTab *tab, MdPtrCount *entry, size_t dim,
	const char *ext, void *ptr);

typedef struct {
	MDCLASS_VAR;
	void *ptr;
} MdPtrClass;

MdPtrClass *MdPtrClass_copy (MdPtrClass *tab, size_t dim,
	const char *ext, void *ptr);
void MdPtrClass_add (MdCountTab *tab, MdPtrClass *entry, size_t dim);
void MdPtrClass_xadd (MdCountTab *tab, MdPtrClass *entry, size_t dim,
	const char *ext, void *ptr);

/*	Z�hlgruppe
*/

typedef struct MdCntGrpStruct {
	struct MdCntGrpStruct *next;	/* N�chste Definition */
	MdClass *cdef;		/* Klassifikationsdefinition */
	int flag;		/* Selektionsflag */
	int idx;		/* Selektionsindex */
} MdCntGrp;


/*	Z�hltabellen
*/

mdaxis *md_classaxis (const char *name, ...);
mdaxis *md_ctabaxis (IO *io, MdCountTab *stab);
mdmat *md_ctab (const char *titel, const char *def,
	MdCountTab *gtab, MdCount *counter);
mdmat *md_ioctab (const char *titel, IO *io,
	MdCountTab *gtab, MdCount *counter);

int md_ctabinit (mdmat *md, MdCountList *list);
void md_count (mdmat *tab, const void *data);
void md_showcnt (IO *io, MdCountTab *tab);
void md_count_add (MdCountList *clist, mdaxis *x, char *ptr);

extern void (*md_count_hook) (MdCountList *clist, mdmat *md);

extern MdCount *stdcount;	/* Standardz�hler mit long - Werten */

void MdSetup_count (void);

/*	Z�hldefinitionsstruktur
*/

typedef struct {
	MDCLASS_VAR;
	EfiObj *expr;
	EfiObj *obj;
} MdEfiClass;

typedef struct {
	REFVAR;
	EfiObj *obj;	/* Basisobjekt */
	char *pfx;	/* Prefix f�r Klassifikationen */
	VecBuf cltab;	/* Klassifikationstabelle */
} MdCntDef;

MdCntDef* MdCntDef_create (EfiType *type, const char *pfx);
void MdCntDef_expr (EfiFunc *funv, void *rval, void **arg);
void MdCntDef_cadd (EfiFunc *func, void *rval, void **arg);

#endif	/* EFEU/mdcount.h */
