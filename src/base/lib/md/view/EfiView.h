/*
Sicht auf ein Efeu-Datenobjekt

$Copyright (C) 2007 Erich Frühstück
This file is part of EFEU.

EFEU is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

EFEU is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public
License along with EFEU; see the file COPYING.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef	EFEU_EfiView_h
#define	EFEU_EfiView_h	1

#include <EFEU/EfiPar.h>
#include <EFEU/object.h>
#include <EFEU/stdtype.h>
#include <EFEU/mktype.h>
#include <EFEU/Info.h>
#include <EFEU/TimeRange.h>
#include <EFEU/EDBFilter.h>
#include <EFEU/StatData.h>

typedef struct {
	uint64_t id;	/* Identifikationsnummer */
	uint64_t rec;	/* Datensatznummer */
	uint64_t num;	/* Ereignisnummer */
} EfiViewSeq;

extern EfiType Type_EfiViewSeq;
void EfiViewSeq_setup(void);

typedef struct EfiView EfiView;

struct EfiView {
	REFVAR;			/* Referenzvariablen */
	EfiVarTab *vtab;	/* Variablentabelle */
	EfiObj *data;		/* Datenobjekt */
	EfiObj *base;		/* Basisobjekt für Methode */
	EDB *edb;		/* EDB-Dateneingabe */

	VecBuf range_buf;	/* Zeitbereich */
	TimeRange *range;	/* Zeitbereichsvektor */
	int range_dim;		/* Zeitbereichsdimension */
	int range_idx;		/* Zeitbereichsindex */

	EfiStruct *st;		/* Struktureinträge */
	EfiFunc *func;		/* Konstruktionsfunktion */
	EfiObj *tg;		/* Zielobjekt */

	EfiViewSeq seq;		/* Ablaufsequenz */
	unsigned date;		/* Ereignisdatum */
	double weight;		/* Basisgewicht */
	VarianceData var;	/* Varianzdaten */

	void (*eval_total) (EfiView *view);
	void (*eval_range) (EfiView *view, TimeRange *range);
	void *eval_par;
	EfiObj *eval_obj;

	size_t nsave;	/* Zahl der geschriebenen Datensätze */
	IO *out;	/* Ausgabestruktur */
};

EfiView *EfiView_create (EfiObj *base, const char *method);
void EfiView_show (EfiView *view, IO *out);
void EfiView_list (IO *out, const char *pfx, EfiVarTab *tab);
void EfiView_var (EfiView *view, const char *def);

void EfiView_process (EfiView *view);
void EfiView_event (EfiView *view, unsigned dat, double val);

void SetupEfiView (void);

typedef struct EfiViewPar EfiViewPar;

struct EfiViewPar {
	EPC_VAR;
	char *syntax;
	void (*create) (EfiView *view, const char *opt, const char *arg);
};

void EfiView_info (IO *io, const void *data);

extern EfiPar EfiPar_view;
extern EfiViewPar EfiView_record;
extern EfiViewPar EfiView_element;
extern EDBFilter EDBFilter_view;

#define	EFI_VIEW(type, name, syntax, create, label) \
	{ EPC_DATA(&EfiPar_view, type, name, label, EfiView_info), \
		syntax, create }

#endif	/* EFEU/EfiView.h */
