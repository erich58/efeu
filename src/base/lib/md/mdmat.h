/*
Arbeiten mit multidimensionalen Matrizen

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

#ifndef	EFEU_mdmat_h
#define	EFEU_mdmat_h	1

#define	MSG_MDMAT	"mdmat"

#include <EFEU/object.h>
#include <EFEU/konvobj.h>
#include <EFEU/pconfig.h>
#include <EFEU/refdata.h>
#include <EFEU/EDB.h>

#define	MD_MAGIC0	0xEFD0		/* Alte Kennung (Revers Byteorder) */
#define	MD_MAGIC1	0xEFD1		/* Datenmatrix, Version 1 */
#define	MD_MAGIC2	0xEFD2		/* Datenmatrix, Version 2 */
#define	MD_EOFMARK	"*EOF"		/* Eofmarke */


/*	Initialisierung
*/

void SetupMdMat (void);
void SetupMdTest (void);


/*	Typedefinition f�r multidimensionale Matrizen
*/

EfiType *mdtype (const char *str);
char *type2str (const EfiType *type);



/*	Struktur f�r Achsenbezeichner
*/

typedef struct {
	char *name;	/* Achsenbezeichner */
	unsigned flags;	/* Selektionsflags */
} mdindex;


/*	Achsenstruktur
*/

typedef struct mdaxis_struct mdaxis;

struct mdaxis_struct {
	mdaxis *next;	/* Verweis auf n�chstes Element */
	size_t size;	/* Datengr��e eines Elementes */
	size_t dim;	/* Zahl der Elemente */
	char *name;	/* Achsenname */
	mdindex *idx;	/* Indexvektor */
	unsigned flags;	/* Steuerflags */
	void *priv;	/* Private Referenzdaten */
};

mdaxis *new_axis (size_t dim);
mdaxis *cpy_axis (mdaxis *axis, unsigned mask);
void del_axis (mdaxis *axis);

#define	MDFLAG_LOCK	1	/* Index ist gesperrt */
#define	MDFLAG_BASE	2	/* Basis f�r Indexzahlen */
#define	MDFLAG_HIDE	4	/* Index nicht ausgeben */
#define	MDFLAG_TEMP	8	/* Tempor�re Sperre */

#define	MDXFLAG_MARK	1	/* Markierte Achse */
#define	MDXFLAG_TIME	2	/* Zeitachse */
#define	MDXFLAG_HIDE	4	/* Achsenlabel unterdr�cken */
#define	MDXFLAG_TEMP	8	/* Tempor�re Markierung */


/*	Achsenvergleich
*/

int cmp_axis (mdaxis *a, mdaxis *b, int flag);

#define	MDXCMP_NAME	1	/* Achsennamen vergleichen */
#define	MDXCMP_DIM	2	/* Dimensionen vergleichen */
#define	MDXCMP_IDX	4	/* Indexbezeichner vergleichen */
#define	MDXCMP_ALL	7	/* Alle Parameter Testen */


/*	Multidimensionale Matrix
*/

typedef struct {
	REFVAR;		/* Referenzz�hler */
	char *title;	/* Titel der Matrix */
	EfiType *type;	/* Datentype */
	mdaxis *axis;	/* Achsenkette */
	size_t size;	/* Speicherbedarf f�r Datenfeld */
	void *data;	/* Datenfeld */
	void *x_priv;	/* Private Daten */
	void *x_data;	/* Referenzobjekt f�r Daten */
} mdmat;

extern RefType md_reftype;

/*
Der Makro $1 erh�ht den Referenzz�hler der Datenmatrix um 1
*/

#define	ref_mdmat(x)	rd_refer(x)

/*
Der Makro $1 verringert den Referenzz�hler der Datenmatrix um 1
und gibt die Datenstruktur bei Bedarf frei.
*/

#define	del_mdmat(x)	rd_deref(x)


/*	Datenstrukturen anfordern/kopieren/freigeben
*/

mdmat *new_mdmat (void);
mdmat *cpy_mdmat (const mdmat *md, unsigned mask);
mdmat *md_skalar (const EfiType *type, const void *data);


/*	Hilffunktionen zur Listenzerlegung
*/

typedef struct mdlist_struct mdlist;

struct mdlist_struct {
	mdlist *next;	/* Verweis auf n�chstes Element */
	char *name;	/* Achsenname */
	char *option;	/* Option */
	size_t dim;	/* Zahl der Listenelemente */
	char **list;	/* Wertepointer f�r Liste */
	char **lopt;	/* Optionsangaben zur Liste */
};


#define	MDLIST_NAMEOPT	0x1	/* Name mit Option */
#define	MDLIST_LISTOPT	0x2	/* Liste mit Option */
#define	MDLIST_ANYOPT	0x3	/* Alle Optionen */
#define	MDLIST_NEWOPT	0x4	/* Achsenerweiterung */

mdlist *io_mdlist (IO *io, int flag);
void del_mdlist (mdlist *list);

mdlist *str2mdlist (const char *str, int flag);
mdlist *mdlistcmp (const char *name, int depth, mdlist *list);


/*	Hilfsfunktionen
*/

size_t md_dim (mdaxis *axis);
size_t md_size (mdaxis *axis, size_t size);
void md_alloc (mdmat *md);
extern void *(*md_alloc_hook) (size_t size);

void md_allflag(mdmat *md, unsigned mask,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi);
void md_setflag(mdmat *md, const char *def, unsigned mask,
	unsigned (*fx) (unsigned old, int flag, unsigned val), unsigned vx,
	unsigned (*fi) (unsigned old, int flag, unsigned val), unsigned vi);

unsigned mdsf_mark (unsigned old, int flag, unsigned val);
unsigned mdsf_lock (unsigned old, int flag, unsigned val);
unsigned mdsf_clear (unsigned old, int flag, unsigned val);
unsigned mdsf_toggle (unsigned old, int flag, unsigned val);


/*	Ein - Ausgabe von multidim. Matrizen
*/

EfiFunc *get_iofunc(const EfiType *type, const char *name, int flag);

mdmat *md_read (IO *io, const char *par);
mdmat *md_load (IO *io, const char *def, const char *odef);
mdmat *md_fload (const char *name, const char *def, const char *odef);
mdmat *md_reload (mdmat *md, const char *def, const char *odef);

mdmat *edb2md (EDB *edb);
EDB *md2edb (mdmat *md);
void Setup_edb2md (void);

mdmat *md_gethdr (IO *io);
void md_tsteof (IO *io);

void md_walk (mdmat *md, const char *def,
	void (*visit)(EfiType *type, void *data));
void md_xwalk (mdmat *md, const char *def,
	void (*visit)(EfiType *type, void *data, void *base));
void md_twalk (mdmat *md, const char *def, int lag,
	void (*visit)(EfiType *type, void *data, void *base));

void md_puthdr (IO *io, mdmat *md, unsigned mask);
void md_putdata (IO *io, const EfiType *type, mdaxis *axis,
	unsigned mask, void *data);
void md_puteof (IO *io);

void md_show (IO *io, mdmat *md);
void md_print (IO *io, mdmat *md, const char *par);
void md_save (IO *io, mdmat *md, unsigned mask);
void md_fsave (const char *name, mdmat *md, unsigned mask);
void md_ssave (IO *io, mdmat *md, int (*cmp) (mdindex *a, mdindex *b),
	char **list, size_t dim);

/*	Schnittstelle zu Befehlsinterpreter
*/

extern EfiType Type_mdmat;
extern EfiType Type_mdaxis;
extern EfiType Type_mdidx;

extern mdmat *Buf_mdmat;
extern mdaxis *Buf_mdaxis;
extern mdindex Buf_mdidx;

#define	Obj_mdmat(x)	NewPtrObj(&Type_mdmat, (x))
#define	Obj_mdaxis(x)	NewPtrObj(&Type_mdaxis, (x))

#define	Val_mdmat(x)	((mdmat **) x)[0]
#define	Val_mdaxis(x)	((mdaxis **) x)[0]
#define	Val_mdidx(x)	((mdindex *) x)[0]

void MdSetup_func (void);


EfiFunc *mdfunc_add (const EfiType *type);
EfiFunc *md_addfunc (const char *op, const EfiType *t1, const EfiType *t2);

double MdValSum(mdaxis *x, void *data, EfiKonv *get, unsigned mask);
void MdRound(mdaxis *x, void *data, EfiKonv *get, EfiKonv *set, double val,
	double factor, unsigned mask);


/*	Matrixmanipulationen
*/

void md_paste (mdmat *md, const char *name, const char *fmt, int pos, int n);
void md_change (mdmat *md, const char *name1, const char *name2);

int md_konv (mdmat *md, const EfiType *type);
void md_reduce (mdmat *md, const char *list);
void md_permut (mdmat *md, const char *list);
void md_round (mdmat *md, double factor, unsigned mask);
void md_adjust (mdmat *md, double val, unsigned mask);
mdmat *md_prod (mdmat *md);
mdmat *md_mul (mdmat *m1, mdmat *m2, int flag);
mdmat *md_sum (mdmat *md, const char *str);
mdmat *md_diag (mdmat *md, const char *str);
mdmat *md_create (EfiType *type, const char *def);
mdmat *md_clone (const mdmat *md);
mdmat *md_assign (const char *name, mdmat *m1, mdmat *m2);
mdmat *md_term (EfiVirFunc *func, mdmat *m1, mdmat *m2);
mdmat *md_cat (const char *name, mdmat **md, size_t dim);
void md_korr (mdmat *md, int rflag);


/*	Datenkonvertierung
*/

EfiKonv *Md_KonvDef (EfiKonv *buf, const EfiType *t1, const EfiType *t2);

long Md_getlong(EfiKonv *konv, void *data);
double Md_getdbl(EfiKonv *konv, void *data);
void Md_setlong(EfiKonv *konv, void *data, long int val);
void Md_setdbl(EfiKonv *konv, void *data, double val);

void MF_valsum (EfiFunc *func, void *rval, void **arg);
void MF_leval (EfiFunc *func, void *rval, void **arg);
void MF_xeval (EfiFunc *func, void *rval, void **arg);
void MF_data (EfiFunc *func, void *rval, void **arg);
void MF_label (EfiFunc *func, void *rval, void **arg);

#endif	/* EFEU/mdmat.h */
