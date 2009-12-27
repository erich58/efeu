/*
Arbeiten mit multidimensionalen Matrizen

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

#ifndef	EFEU_mdmat_h
#define	EFEU_mdmat_h	1

#define	MSG_MDMAT	"mdmat"

#include <EFEU/object.h>
#include <EFEU/konvobj.h>
#include <EFEU/pconfig.h>
#include <EFEU/refdata.h>

#define	MD_MAGIC	0xEFD1		/* Kennung einer Datenmatrix */
#define	MD_OLDMAGIC	0xEFD0		/* Alte Kennung (Revers Byteorder) */
#define	MD_EOFMARK	"*EOF"		/* Eofmarke */


/*	Initialisierung
*/

void SetupMdMat (void);
void SetupMdTest (void);


/*	Typedefinition für multidimensionale Matrizen
*/

Type_t *mdtype (const char *str);
char *type2str (const Type_t *type);



/*	Struktur für Achsenbezeichner
*/

typedef struct {
	char *name;	/* Achsenbezeichner */
	unsigned flags;	/* Selektionsflags */
} mdidx_t;


/*	Achsenstruktur
*/

typedef struct md_s {
	struct md_s *next;	/* Verweis auf nächstes Element */
	size_t size;		/* Datengröße eines Elementes */
	size_t dim;		/* Zahl der Elemente */
	char *name;		/* Achsenname */
	mdidx_t *idx;		/* Indexvektor */
	unsigned flags;		/* Steuerflags */
	void *priv;		/* Private Daten */
} mdaxis_t;

mdaxis_t *new_axis (size_t dim);
mdaxis_t *cpy_axis (mdaxis_t *axis, unsigned mask);
void del_axis (mdaxis_t *axis);
int check_axis (mdaxis_t *axis);

#define	MDFLAG_LOCK	1	/* Index ist gesperrt */
#define	MDFLAG_BASE	2	/* Basis für Indexzahlen */
#define	MDFLAG_HIDE	4	/* Index nicht ausgeben */
#define	MDFLAG_TEMP	8	/* Temporäre Sperre */

#define	MDXFLAG_MARK	1	/* Markierte Achse */
#define	MDXFLAG_TIME	2	/* Zeitachse */
#define	MDXFLAG_HIDE	4	/* Achsenlabel unterdrücken */
#define	MDXFLAG_TEMP	8	/* Temporäre Markierung */


/*	Achsenvergleich
*/

int cmp_axis (mdaxis_t *a, mdaxis_t *b, int flag);

#define	MDXCMP_NAME	1	/* Achsennamen vergleichen */
#define	MDXCMP_DIM	2	/* Dimensionen vergleichen */
#define	MDXCMP_IDX	4	/* Indexbezeichner vergleichen */
#define	MDXCMP_ALL	7	/* Alle Parameter Testen */


/*	Multidimensionale Matrix
*/

typedef struct {
	REFVAR;		/* Referenzzähler */
	char *title;	/* Titel der Matrix */
	Type_t *type;	/* Datentype */
	mdaxis_t *axis;	/* Achsenkette */
	size_t size;	/* Speicherbedarf für Datenfeld */
	void *data;	/* Datenfeld */
	void *priv;	/* Private Daten */
} mdmat_t;

extern reftype_t md_reftype;
char *md_ident(mdmat_t *md);

/*
Der Makro $1 erhöht den Referenzzähler der Datenmatrix um 1
*/

#define	ref_mdmat(x)	rd_refer(x)

/*
Der Makro $1 verringert den Referenzzähler der Datenmatrix um 1
und gibt die Datenstruktur bei Bedarf frei.
*/

#define	del_mdmat(x)	rd_deref(x)


/*	Datenstrukturen anfordern/kopieren/freigeben
*/

mdmat_t *new_mdmat (void);
mdmat_t *cpy_mdmat (const mdmat_t *md, unsigned mask);
mdmat_t *md_skalar (const Type_t *type, const void *data);


/*	Hilffunktionen zur Listenzerlegung
*/

typedef struct mdlist_s {
	struct mdlist_s *next;	/* Verweis auf nächstes Element */
	char *name;		/* Achsenname */
	char *option;		/* Option */
	size_t dim;		/* Zahl der Listenelemente */
	char **list;		/* Wertepointer für Liste */
	char **lopt;		/* Optionsangaben zur Liste */
} mdlist_t;


#define	MDLIST_NAMEOPT	0x1	/* Name mit Option */
#define	MDLIST_LISTOPT	0x2	/* Liste mit Option */
#define	MDLIST_ANYOPT	0x3	/* Alle Optionen */
#define	MDLIST_NEWOPT	0x4	/* Achsenerweiterung */

mdlist_t *io_mdlist (io_t *io, int flag);
void del_mdlist (mdlist_t *list);

mdlist_t *mdlist (const char *str, int flag);
mdlist_t *mdlistcmp (const char *name, int depth, mdlist_t *list);


/*	Hilfsfunktionen
*/

size_t md_dim (mdaxis_t *axis);
size_t md_size (mdaxis_t *axis, size_t size);

typedef unsigned (*mdsetflag_t) (unsigned old, int flag, unsigned val);

void md_allflag(mdmat_t *md, unsigned mask,
	mdsetflag_t fx, unsigned vx, mdsetflag_t fi, unsigned vi);
void md_setflag(mdmat_t *md, const char *def, unsigned mask,
	mdsetflag_t fx, unsigned vx, mdsetflag_t fi, unsigned vi);

unsigned mdsf_mark (unsigned old, int flag, unsigned val);
unsigned mdsf_lock (unsigned old, int flag, unsigned val);
unsigned mdsf_clear (unsigned old, int flag, unsigned val);
unsigned mdsf_toggle (unsigned old, int flag, unsigned val);


/*	Ein - Ausgabe von multidim. Matrizen
*/

Func_t *get_iofunc(const Type_t *type, const char *name, int flag);

mdmat_t *md_read (io_t *io, const char *par);
mdmat_t *md_load (io_t *io, const char *def, const char *odef);
mdmat_t *md_fload (const char *name, const char *def, const char *odef);
mdmat_t *md_reload (mdmat_t *md, const char *def, const char *odef);

mdmat_t *md_gethdr (io_t *io);
void md_tsteof (io_t *io);

void md_walk (mdmat_t *md, const char *def, void (*visit)(mdmat_t *md, mdaxis_t *x, int idx));

void md_puthdr (io_t *io, mdmat_t *md, unsigned mask);
void md_putdata (io_t *io, const Type_t *type, mdaxis_t *axis,
	unsigned mask, void *data);
void md_puteof (io_t *io);

void md_print (io_t *io, mdmat_t *md, const char *par);
void md_save (io_t *io, mdmat_t *md, unsigned mask);
void md_fsave (const char *name, mdmat_t *md, unsigned mask);
void md_ssave (io_t *io, mdmat_t *md, int (*cmp) (mdidx_t *a, mdidx_t *b),
	char **list, size_t dim);

/*	Schnittstelle zu Befehlsinterpreter
*/

extern Type_t Type_mdmat;
extern Type_t Type_mdaxis;
extern Type_t Type_mdidx;

extern mdmat_t *Buf_mdmat;
extern mdaxis_t *Buf_mdaxis;
extern mdidx_t Buf_mdidx;

#define	Obj_mdmat(x)	NewPtrObj(&Type_mdmat, (x))
#define	Obj_mdaxis(x)	NewPtrObj(&Type_mdaxis, (x))

#define	Val_mdmat(x)	((mdmat_t **) x)[0]
#define	Val_mdaxis(x)	((mdaxis_t **) x)[0]
#define	Val_mdidx(x)	((mdidx_t *) x)[0]

void MdFuncDef (void);


Func_t *mdfunc_add (const Type_t *type);
Func_t *md_addfunc (const char *op, const Type_t *t1, const Type_t *t2);
Func_t *md_binop (const char *op, const Type_t *t1, const Type_t *t2);
Func_t *md_assignop (const char *op, const Type_t *t1, const Type_t *t2);

double MdValSum(mdaxis_t *x, void *data, Konv_t *get, unsigned mask);
void MdRound(mdaxis_t *x, void *data, Konv_t *get, Konv_t *set, double val,
	double factor, unsigned mask);


/*	Matrixmanipulationen
*/

void md_paste (mdmat_t *md, const char *name, const char *fmt, int pos, int n);
void md_change (mdmat_t *md, const char *name1, const char *name2);

int md_konv (mdmat_t *md, const Type_t *type);
void md_reduce (mdmat_t *md, const char *list);
void md_permut (mdmat_t *md, const char *list);
void md_round (mdmat_t *md, double factor, unsigned mask);
void md_adjust (mdmat_t *md, double val, unsigned mask);
mdmat_t *md_prod (mdmat_t *md);
mdmat_t *md_mul (mdmat_t *m1, mdmat_t *m2, int flag);
mdmat_t *md_sum (mdmat_t *md, const char *str);
mdmat_t *md_diag (mdmat_t *md, const char *str);
mdmat_t *md_create (Type_t *type, const char *def);
mdmat_t *md_assign (const char *name, mdmat_t *m1, mdmat_t *m2);
mdmat_t *md_term (VirFunc_t *func, mdmat_t *m1, mdmat_t *m2);
mdmat_t *md_cat (const char *name, mdmat_t **md, size_t dim);
void md_korr (mdmat_t *md, int rflag);


/*	Daten auswerten
*/

typedef struct {
	void *(*init) (void *par, Type_t *type);
	void *(*exit) (void *par);
	void *(*newidx) (const void *idx, char *name);
	void (*clridx) (void *idx);
	void (*start) (void *par, const void *idx);
	void (*end) (void *par);
	void (*data) (void *par, Type_t *type, void *ptr, void *base);
} mdeval_t;

void *md_eval(mdeval_t *eval, void *par, mdmat_t *md, unsigned mask, unsigned base, int lag);


/*	Teststruktur für Achsennamen
*/

typedef struct mdtest_s {
	struct mdtest_s *next;	/* Für Verkettungen */
	char *pattern;		/* Musterkennung */
	int flag;		/* Negationsflag */
	size_t minval;		/* Minimalwert */
	size_t maxval;		/* Maximalwert */
	int (*cmp) (struct mdtest_s *t, const char *s, size_t n);
} mdtest_t;

mdtest_t *new_test (const char *def, size_t dim);
void del_test (mdtest_t *test);
mdtest_t *mdmktestlist (const char *list, size_t dim);
mdtest_t *mdtestlist (char **list, size_t ldim, size_t dim);
int mdtest (mdtest_t *test, const char *name, size_t idx);


/*	Datenkonvertierung
*/

Konv_t *Md_KonvDef (Konv_t *buf, const Type_t *t1, const Type_t *t2);

long Md_getlong(Konv_t *konv, void *data);
double Md_getdbl(Konv_t *konv, void *data);
void Md_setlong(Konv_t *konv, void *data, long int val);
void Md_setdbl(Konv_t *konv, void *data, double val);

void MF_valsum (Func_t *func, void *rval, void **arg);
void MF_leval (Func_t *func, void *rval, void **arg);
void MF_xeval (Func_t *func, void *rval, void **arg);
void MF_data (Func_t *func, void *rval, void **arg);
void MF_label (Func_t *func, void *rval, void **arg);

/*	Bibliothekinitialisierung
*/

extern void *mdmat;

#endif	/* EFEU/mdmat.h */
