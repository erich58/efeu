/*
EFEU-Datenbank

$Header <EFEU/$1>

$Copyright (C) 2004 Erich Frühstück
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

#ifndef	EFEU_EDB_h
#define	EFEU_EDB_h	1

#include <EFEU/object.h>
#include <EFEU/CmpDef.h>
#include <EFEU/refdata.h>
#include <EFEU/Info.h>
#include <EFEU/Debug.h>

extern LogControl *edb_err;
extern LogControl *edb_note;
extern LogControl *edb_info;
extern LogControl *edb_debug;

typedef struct {
	REFVAR;		/* Referenzzähler */
	char *head;	/* Initialisierungsdefinition */
	char *desc;	/* Beschreibungstext */
	EfiObj *obj;	/* Datenobjekt */

	int save;	/* Flag für Zurückschreiben */
	void *ipar;	/* Eingabeparameter */
	int (*read) (EfiType *type, void *data, void *par);
	size_t nread;	/* Zahl der gelesenen Datensätze */

	void *opar;	/* Ausgabeparameter */
	size_t (*write) (EfiType *type, void *data, void *par);
	size_t nwrite;	/* Zahl der geschriebenen Datensätze */
} EDB;

EfiType *edb_type (const char *def);
EDB *edb_alloc (EfiObj *obj, char *desc);
EDB *edb_create (const EfiType *type);
EDB *edb_share (EDB *base);
EDB *edb_fopen (const char *path, const char *name);
EDB *edb_popen (const char *cmd);
EDB *edb_open (IO *io);
EDB *edb_meta (IO *io, char *path);

extern EfiType Type_EDB;

void edb_data (EDB *edb, IO *io, const char *mode);
void edb_expr (EDB *edb, EfiObj *expr);
void *edb_read (EDB *edb);
void edb_unread (EDB *edb);
void edb_input (EDB *edb, int (*read) (EfiType *, void *, void *), void *par);
void edb_closein (EDB *edb);
void edb_tmpload (EDB *edb);

/*
output mode
*/

typedef struct EDBPrintModeStruct EDBPrintMode;

struct EDBPrintModeStruct {
	char *name;
	void (*init) (EDB *edb, EDBPrintMode *mode, IO *io);
	IO *(*filter) (IO *base);
	int header;
	size_t split;
	void *par;
};

EDBPrintMode *edb_pmode (EDBPrintMode *pmode, EDB *edb, const char *mode);

typedef struct EDBPrintDefStruct EDBPrintDef;

struct EDBPrintDefStruct {
	char *name;
	char *syntax;
	void (*set) (EDBPrintMode *mode, const EDBPrintDef *def,
		const char *opt, const char *arg);
	char *par;
	char *desc;
};

char *EDBPrintDef_psub (const char *fmt, const char *opt, const char *arg);

void EDBPrintDef_alias (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_null (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_binary (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_plain (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_label (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_data (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_fmt (EDBPrintMode *mode, const EDBPrintDef *def,
	const char *opt, const char *arg);
void EDBPrint_data_init (EDB *edb, EDBPrintMode *mode, IO *io);

void AddEDBPrintDef (EDBPrintDef *def, size_t dim);
EDBPrintDef *GetEDBPrintDef (const char *name);
void ListEDBPrintDef (IO *out);
void ShowEDBPrintDef (IO *io, EDBPrintDef *def, int verbosity);
void SetupEDBPrint (void);
void EDBPrintInfo (InfoNode *info);

void edb_head (const EDB *edb, IO *io, int verbosity);
void edb_vlist (const EDB *edb, const char *pfx, IO *io);
void edb_fout (EDB *edb, const char *name, const char *mode);
void edb_out (EDB *edb, IO *io, const char *mode);
void edb_closeout (EDB *edb);
void edb_setout (EDB *edb, IO *out, EDBPrintMode *mode);
void edb_write (EDB *edb);

int edb_copy (EDB *edb, int lim);

/*	Zuweisungsstruktur
*/

typedef struct EDBAssignStruct {
	REFVAR;
	struct EDBAssignStruct *next;
	struct EDBAssignStruct *sub;
	EfiStruct *var;
	unsigned offset;
	EfiObj *base;
	EfiStruct *st;
} EDBAssign;

EDBAssign *NewEDBAssign (EDBAssign *next, EfiObj *base, EfiStruct *st,
	EfiStruct *var, unsigned offset);
void EDBAssignData (EDBAssign *assign, void *tg, void *src);
void EDBAssignFunc (EDBAssign *assign, EfiType *tg, EfiType *base);
void PrintEDBAssign (IO *out, EDBAssign *assign);

/*	Aggregationsstruktur
*/

typedef struct EDBAggregateStruct {
	REFVAR;
	struct EDBAggregateStruct *next;
	struct EDBAggregateStruct *sub;
	EfiStruct *var;
	EfiFunc *f_aggregate;
	unsigned offset;
} EDBAggregate;

EDBAggregate *NewEDBAggregate (EDBAggregate *next, const char *op,
	EfiStruct *var, unsigned offset);
void EDBAggregateData (EDBAggregate *assign, void *tg, void *src);
void EDBAggregateFunc (EDBAggregate *assign, const char *op,
	EfiType *tg, EfiType *base);


EDB *edb_filter (EDB *base, const char *def);
EDB *edb_script (EDB *base, IO *def);
EDB *edb_trans (EDB *base, const char *def);
EDB *edb_join (EDB *edb1, EDB *edb2, const char *def);
EDB *edb_paste (EDB *base, EDB *sub);
EDB *edb_conv (EDB *base, EfiType *type);
EDB *edb_xconv (EDB *base, const char *name);

extern size_t edb_sort_space;
EDB *edb_sort (EDB *edb, CmpDef *cmp);
EDB *edb_merge (CmpDef *cmp, EDB **tab, size_t dim);
EDB *edb_cat (EDB **tab, size_t dim);

void EfiVec_load (EfiVec *vec, const char *name,
	const char *mode, const char *filter);
void EfiVec_save (EfiVec *vec, const char *name,
	const char *mode, const char *desc);

void SetupEDBInfo (void);

void SetupEDB (void);

#endif
