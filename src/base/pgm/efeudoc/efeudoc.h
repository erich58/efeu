/*
Dokumentgenerator

$Copyright (C) 1999 Erich Frühstück
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

#ifndef	efeudoc_h
#define	efeudoc_h	1

#include <EFEU/io.h>
#include <EFEU/refdata.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/Info.h>
#include <EFEU/object.h>
#include <efeudoc/DocOut.h>
#include <efeudoc/DocCtrl.h>

#ifndef	MSG_DOC
#define	MSG_DOC	"Doc"
#endif

extern int DocSkipSpace (IO *in, int flag);
extern int DocSkipWhite (IO *in);

extern int DocParseNum (IO *in);
extern char *DocParseFlags (IO *in);
extern char *DocParseName (IO *in, int key);
extern char *DocParseMacArg (IO *in);
extern char *DocParseLine (IO *in, int flag);
extern char *DocParsePar (IO *in);
extern char *DocParseExpr (IO *in);
extern char *DocParseRegion (IO *in, const char *delim);
extern char *DocParseArg (IO *in, int beg, int end, int flag);
extern char *DocParseBlock (IO *in, int mode, const char *beg,
	const char *end, const char *toggle);

extern void DocCopy (IO *in, IO *out);
extern void DocExpand (IO *in, IO *out);
extern int DocVerb (IO *in, IO *out);
extern int DocSymbol (IO *in, IO *out);


typedef struct DocStruct Doc;

typedef struct {
	unsigned type : 16;
	unsigned par_type : 16;
	unsigned indent : 16;
	unsigned depth : 16;
	unsigned cpos : 21;
	unsigned hmode : 2;
	unsigned pflag : 1;
	char mark_key;
	Stack *mark;
	char *item;
	void (*par_beg) (Doc *doc);
	void (*par_end) (Doc *doc);
} DocEnv;

struct DocStruct {
	REFVAR;			/* Referenzvariablen */
	DocType *type;		/* Ausgabetype */
	char *name;		/* Dokumentname */
	StrBuf buf;		/* Ausgabebuffer */
	IO *out;		/* Ausgabestruktur */
	unsigned stat : 1;	/* Statusflag */
	unsigned nl : 15;	/* Zahl der gelesenen Leerzeilen */
	unsigned indent : 15;	/* Aktuelle Einrückung */
	DocEnv env;		/* Aktuelle Umgebung */
	Stack *env_stack;	/* Stack mit Umgebungen */
	Stack *cmd_stack;	/* Stack mit Befehlstabellen */
};

Doc *Doc_create (const char *type);

IO *Doc_open (const char *path, const char *name, int flag);
IO *Doc_preproc (IO *io);

char *Doc_lastcomment (Doc *doc);
void Doc_rem (Doc *doc, const char *fmt, const char *argdef, ...);
void Doc_close (Doc *doc);
void Doc_copy (Doc *doc, IO *in);
void Doc_key (Doc *doc, IO *in, int c);
char *Doc_expand (Doc *doc, IO *in, int flag);
char *Doc_xexpand (Doc *doc, IO *in);
void Doc_verb (Doc *doc, IO *in, int base, int alt, int wrap);
void Doc_scopy (Doc *doc, const char *str);
void Doc_mcopy (Doc *doc, char *str);
void Doc_xcopy (Doc *doc, IO *in, int c);
void Doc_input (Doc *doc, const char *opt, IO *in);
void Doc_include (Doc *doc, const char *opt, const char *name);
void Doc_str (Doc *doc, const char *str);
void Doc_char (Doc *doc, int32_t c);
void Doc_symbol (Doc *doc, const char *name, const char *def);
void Doc_psub (Doc *doc, IO *in);
void Doc_eval (Doc *doc, IO *in, const char *expr);
void Doc_sig (Doc *doc, int sig, IO *in);
void Doc_minus (Doc *doc, IO *in);
void Doc_dots (Doc *doc, IO *in);
IO *Doc_subin (IO *io, const char *key);

void Doc_start (Doc *doc);
void Doc_hmode (Doc *doc);
void Doc_par (Doc *doc);
void Doc_stdpar (Doc *doc, int type);
void Doc_newline (Doc *doc);

int DocMark_type (int c, int def);
int DocMark_beg (Stack **stack, IO *out, IO *in);
int DocMark_end (Stack **stack, IO *out, int key);

void Doc_begmark (Doc *doc, IO *in);
void Doc_endmark (Doc *doc);
void Doc_nomark (Doc *doc);
void Doc_unmark (Doc *doc);
void Doc_remark (Doc *doc);

void Doc_item (Doc *doc, int type);
int Doc_islist (Doc *doc);
void Doc_endlist (Doc *doc);
void Doc_newenv (Doc *doc, int depth, ...);
void Doc_endenv (Doc *doc);
void Doc_endall (Doc *doc, int mode);
void Doc_tab (Doc *doc, IO *in, const char *opt, const char *arg);


/*	Dokumentmakros
*/

typedef struct {
	char *name;	/* Makroname */
	char *desc;	/* Makrobeschreibung */
	char *fmt;	/* Makroformat */
} DocMac;

void DocMac_clean (DocMac *mac);
void DocMac_print (IO *io, DocMac *mac, int mode);


/*	Variablen- und Makrotabellen
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Tabellenname */
	EfiVarTab *var;	/* Variablentabelle */
	VecBuf mtab;	/* Makrotabelle */
} DocTab;

extern RefType DocTab_reftype;
DocTab *DocTab_create (const char *name);
void DocTab_setmac (DocTab *tab, char *name, char *desc, char *fmt);
DocMac *DocTab_getmac (DocTab *tab, const char *name);

void DocTab_def (DocTab *tab, IO *in, StrBuf *buf);
void DocTab_load (DocTab *tab, IO *io);
void DocTab_fload (DocTab *tab, const char *name);


/*	Abfragefunktionen
*/

void DocMacInfo (const char *name, const char *desc);

DocTab *Doc_gettab (Doc *doc, const char *name);
DocMac *Doc_getmac (Doc *doc, const char *name);
EfiVarTab *Doc_vartab (Doc *doc);
EfiObj *Doc_getvar (Doc *doc, const char *name);
void Doc_pushvar (Doc *doc);
void Doc_popvar (Doc *doc);
void Doc_cmd (Doc *doc, IO *in);
void Doc_mac (Doc *doc, IO *in);

/*	Initialisierung
*/

extern DocTab *GlobalDocTab;
extern EfiType Type_Doc;
extern char *DocPath;
extern char *DocName;
extern char *DocDir;
extern char *DocStyle;
extern char *CFGPath;

char *ListArg_str (EfiObjList *list, int n);
int ListArg_int (EfiObjList *list, int n);
void DocFunc_section (EfiFunc *func, void *rval, void **arg);
void SetupDoc (void);

#endif	/* efeudoc.h */
