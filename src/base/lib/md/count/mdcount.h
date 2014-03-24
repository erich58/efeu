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
#include <EFEU/TimeRange.h>

/*	Zählerdefinition
*/

typedef struct MdCount MdCount;

#define	MDCOUNT_VAR \
	char *name; \
	char *type; \
	char *desc; \
	int (*set) (MdCount *cnt, void *buf, const void *data); \
	void (*init) (MdCount *cnt, void *data, size_t *idx, size_t dim); \
	void (*add) (MdCount *cnt, void *data, void *buf)

struct MdCount {
	MDCOUNT_VAR;
};

typedef struct MdCountList MdCountList;

struct MdCountList {
	REFVAR;
	MdCount *cnt;
	EfiType *type;
	size_t offset;
	int flag;
	void *data;
	MdCountList *next;
};

MdCountList *MdCountList_create (MdCount *cnt);
MdCountList *MdCountList_add (MdCountList *list, MdCount *cnt);

typedef struct MdCountPar MdCountPar;

struct MdCountPar {
	REFVAR;
	StrPool *pool;	/* Stringpool */
	VecBuf vtab;	/* Variablentabelle */
	VecBuf ctab;	/* Klassifikationstabelle */
	TimeRange *time_range;	/* Zeitbereich */
	int time_dim;		/* Zahl der Zeitbereiche */
	int time_idx;		/* Aktueller Zeitindex */
	mdaxis *axis;		/* Vordefinierte Achsen */
	mdmat *md;		/* Datenmatrix */
	EfiObj *obj;		/* Zählobjekt */
	EDB *edb;		/* Ausgabedatenbank */
	void *cpar;		/* Zählparameter */
	int (*count) (MdCountPar *par, TimeRange *range);
	uint64_t recnum;
	uint64_t event;
	double weight;		/* Hochrechnungsgewicht */
};
	
MdCountPar *MdCountPar_create (void);
void MdCountPar_range (MdCountPar *par, const char *range);
void MdCountPar_init (MdCountPar *par, EfiObj *obj, const char *method);
int MdCountPar_count (MdCountPar *par);
void MdCountPar_event (MdCountPar *par, unsigned dat, double weight);

MdCount *MdCount_get (MdCountPar *tab, const char *name);
MdCountList *MdCountList_get (MdCountPar *tab, const char *name);
void AddMdCount (MdCountPar *tab, void *entry);
void MdCount_add (MdCountPar *tab, MdCount *entry, size_t dim);
void MdCountInfo (InfoNode *info, MdCountPar *tab);

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
void MdShowClass (IO *io, MdCountPar *tab, const char *plist);
void MdClassInfo (InfoNode *info, MdCountPar *tab);

void AddMdClass (MdCountPar *tab, void *entry);
void MdClass_add (MdCountPar *class, MdClass *tab, size_t dim);
MdClass *MdClass_get (MdCountPar *tab, const char *name);

MdClass *md_subclass (MdClass *base, const char *def);

/*	Pointerzähler und Pointerklassen
*/

typedef struct {
	MDCOUNT_VAR;
	void *ptr;
} MdPtrCount;

MdPtrCount *MdPtrCount_copy (MdPtrCount *tab, size_t dim,
	const char *ext, void *ptr);
void MdPtrCount_add (MdCountPar *tab, MdPtrCount *entry, size_t dim);
void MdPtrCount_xadd (MdCountPar *tab, MdPtrCount *entry, size_t dim,
	const char *ext, void *ptr);

typedef struct {
	MDCLASS_VAR;
	void *ptr;
} MdPtrClass;

MdPtrClass *MdPtrClass_copy (MdPtrClass *tab, size_t dim,
	const char *ext, void *ptr);
void MdPtrClass_add (MdCountPar *tab, MdPtrClass *entry, size_t dim);
void MdPtrClass_xadd (MdCountPar *tab, MdPtrClass *entry, size_t dim,
	const char *ext, void *ptr);

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
mdaxis *md_ctabaxis (IO *io, MdCountPar *stab);
mdmat *md_ctab (const char *titel, const char *def,
	MdCountPar *gtab, MdCount *counter);
mdmat *md_ioctab (const char *titel, IO *io,
	MdCountPar *gtab, MdCount *counter);

int md_ctabinit (mdmat *md, MdCountList *list);
void md_count (mdmat *tab, const void *data);
void md_showcnt (IO *io, MdCountPar *tab);
void md_count_add (MdCountList *clist, mdaxis *x, char *ptr);

extern void (*md_count_hook) (MdCountList *clist, mdmat *md);

extern MdCount *stdcount;	/* Standardzähler mit long - Werten */

void MdSetup_count (void);

/*	Zähldefinitionsstruktur
*/

typedef struct {
	MDCLASS_VAR;
	EfiObj *expr;
	EfiObj *obj;
} MdEfiClass;

typedef struct {
	REFVAR;
	EfiObj *obj;	/* Basisobjekt */
	char *pfx;	/* Prefix für Klassifikationen */
	VecBuf cltab;	/* Klassifikationstabelle */
} MdCntDef;

MdCntDef* MdCntDef_create (EfiType *type, const char *pfx);
void MdCntDef_expr (EfiFunc *funv, void *rval, void **arg);
void MdCntDef_cadd (EfiFunc *func, void *rval, void **arg);

/*
Datenobjekt für Zählungen vorbereiten
*/

void MdCntObj (MdCountPar *tab, EfiObj *obj, char *name, char *desc);
void MdCntObjClass (MdCountPar *tab, EfiObj *obj, char *name, char *desc);

extern double MdCountWeight;

#endif	/* EFEU/mdcount.h */
