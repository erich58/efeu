/*	Auszählen von mehrdimensionalen Datenmatrizen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#ifndef	MDCOUNT_H
#define	MDCOUNT_H	1

#include <EFEU/mdmat.h>
#include <EFEU/Info.h>

/*	Zählerdefinition
*/

typedef int (*MdSet_t) (const void *data);
typedef void (*MdAdd_t) (void *data);
typedef void (*MdInit_t) (void *data, size_t *idx, size_t dim);

typedef struct {
	char *name;
	char *type;
	char *desc;
	MdSet_t set;
	MdInit_t init;
	MdAdd_t add;
} MdCount_t;

void MdCountInfo (InfoNode_t *info, xtab_t *tab);


/*	Klassifikationsdefinitionen
*/

typedef struct MdClass_s MdClass_t;
typedef int (*MdClassify_t) (MdClass_t *cdef, const void *ptr);

#define	MDCLASS_VAR \
	char *name; \
	char *desc; \
	size_t dim; \
	Label_t *label; \
	MdClassify_t classify

struct MdClass_s {
	MDCLASS_VAR;
};

#define	 MdClassify(cdef, ptr)	\
	(((cdef) && (cdef)->classify) ? (cdef)->classify((cdef), ptr) : 0)

extern char *MdClassListFormat;
extern char *MdClassPrintHead;
extern char *MdClassPrintEntry;
extern char *MdClassPrintFoot;
extern int MdClassPrintLimit;

void MdClassList (io_t *io, MdClass_t *cdef);
void MdClassPrint (io_t *io, MdClass_t *cdef);
void MdShowClass (io_t *io, xtab_t *tab, const char *plist);
void MdClassInfo (InfoNode_t *info, xtab_t *tab);


/*	Unterklassen
*/

typedef struct {
	MDCLASS_VAR;
	MdClass_t *main;
	int *idx;
} MdSubClass_t;

MdSubClass_t *MdSubClass (MdClass_t *class);


/*	Zählgruppe
*/

typedef struct cgrp_s {
	struct cgrp_s *next;	/* Nächste Definition */
	MdClass_t *cdef;	/* Klassifikationsdefinition */
	int flag;		/* Selektionsflag */
	int idx;		/* Selektionsindex */
} cgrp_t;


/*	Zähltabellen
*/

mdaxis_t *md_classaxis (const char *name, ...);
mdaxis_t *md_ctabaxis (io_t *io, void *stab);
mdmat_t *md_ctab (const char *titel, const char *def, void *gtab, MdCount_t *counter);
mdmat_t *md_ioctab (const char *titel, io_t *io, void *gtab, MdCount_t *counter);

void md_ctabinit (mdmat_t *md, MdCount_t *counter);
void md_count (mdmat_t *tab, const void *data);
void md_showcnt (io_t *io, xtab_t *tab);

extern MdCount_t *stdcount;	/* Standardzähler mit long - Werten */

#endif	/* MDCOUNT_H */
