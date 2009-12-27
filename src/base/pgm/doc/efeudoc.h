/*	Dokumentgenerator
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	_efeudoc_h
#define	_efeudoc_h	1

#include <EFEU/io.h>
#include <EFEU/refdata.h>
#include <EFEU/ioctrl.h>
#include <EFEU/parsub.h>
#include <EFEU/mstring.h>
#include <EFEU/stack.h>
#include <EFEU/Info.h>
#include <EFEU/object.h>
#include "DocOut.h"
#include "DocCtrl.h"

#ifndef	MSG_DOC
#define	MSG_DOC	"Doc"
#endif

extern int DocSkipSpace (io_t *in, int flag);
extern int DocSkipWhite (io_t *in);

extern int DocParseNum (io_t *in);
extern char *DocParseName (io_t *in, int key);
extern char *DocParseMacArg (io_t *in);
extern char *DocParseLine (io_t *in, int flag);
extern char *DocParsePar (io_t *in);
extern char *DocParseExpr (io_t *in);
extern char *DocParseRegion (io_t *in, const char *delim);
extern char *DocParseArg (io_t *in, int beg, int end, int flag);
extern char *DocParseBlock (io_t *in, int mode, const char *beg,
	const char *end, const char *toggle);

extern void DocCopy (io_t *in, io_t *out);
extern void DocExpand (io_t *in, io_t *out);
extern int DocVerb (io_t *in, io_t *out);
extern int DocSymbol (io_t *in, io_t *out);


typedef struct Doc_s Doc_t;

typedef struct {
	unsigned type : 16;
	unsigned par_type : 16;
	unsigned indent : 16;
	unsigned depth : 16;
	unsigned cpos : 21;
	unsigned hmode : 2;
	unsigned pflag : 1;
	char mark_key;
	stack_t *mark;
	char *item;
	void (*par_beg) (Doc_t *doc);
	void (*par_end) (Doc_t *doc);
} DocEnv_t;

struct Doc_s {
	REFVAR;			/* Referenzvariablen */
	DocType_t *type;	/* Ausgabetype */
	char *name;		/* Dokumentname */
	strbuf_t *buf;		/* Ausgabebuffer */
	io_t *out;		/* Ausgabestruktur */
	unsigned stat : 1;	/* Statusflag */
	unsigned nl : 15;	/* Zahl der gelesenen Leerzeilen */
	unsigned indent : 15;	/* Aktuelle Einrückung */
	DocEnv_t env;		/* Aktuelle Umgebung */
	stack_t *env_stack;	/* Stack mit Umgebungen */
	stack_t *cmd_stack;	/* Stack mit Befehlstabellen */
};

extern reftype_t Doc_reftype;

Doc_t *Doc (const char *type);

io_t *Doc_open (const char *path, const char *name, int flag);

char *Doc_lastcomment (Doc_t *doc);
void Doc_rem (Doc_t *doc, const char *fmt);
void Doc_close (Doc_t *doc);
void Doc_copy (Doc_t *doc, io_t *in);
void Doc_key (Doc_t *doc, io_t *in, int c);
char *Doc_expand (Doc_t *doc, io_t *in, int flag);
char *Doc_xexpand (Doc_t *doc, io_t *in);
void Doc_verb (Doc_t *doc, io_t *in, int base, int alt);
void Doc_scopy (Doc_t *doc, const char *str);
void Doc_mcopy (Doc_t *doc, char *str);
void Doc_xcopy (Doc_t *doc, io_t *in, int c);
void Doc_input (Doc_t *doc, const char *opt, io_t *in);
void Doc_include (Doc_t *doc, const char *opt, const char *name);
void Doc_str (Doc_t *doc, const char *str);
void Doc_char (Doc_t *doc, int c);
void Doc_symbol (Doc_t *doc, const char *name, const char *def);
void Doc_psub (Doc_t *doc, io_t *in);
void Doc_eval (Doc_t *doc, io_t *in, const char *expr);
void Doc_sig (Doc_t *doc, int sig, io_t *in);
void Doc_minus (Doc_t *doc, io_t *in);
io_t *Doc_subin (io_t *io, const char *key);

void Doc_start (Doc_t *doc);
void Doc_hmode (Doc_t *doc);
void Doc_par (Doc_t *doc);
void Doc_stdpar (Doc_t *doc, int type);
void Doc_newline (Doc_t *doc);

int DocMark_type (int c, int def);
int DocMark_beg (stack_t **stack, io_t *out, io_t *in);
int DocMark_end (stack_t **stack, io_t *out, int key);

void Doc_begmark (Doc_t *doc, io_t *in);
void Doc_endmark (Doc_t *doc);
void Doc_nomark (Doc_t *doc);
void Doc_unmark (Doc_t *doc);
void Doc_remark (Doc_t *doc);

void Doc_item (Doc_t *doc, int type);
int Doc_islist (Doc_t *doc);
void Doc_endlist (Doc_t *doc);
void Doc_newenv (Doc_t *doc, int depth, ...);
void Doc_endenv (Doc_t *doc);
void Doc_endall (Doc_t *doc, int mode);
void Doc_tab (Doc_t *doc, io_t *in, const char *height, const char *width);


/*	Dokumentmakros
*/

typedef struct {
	char *name;	/* Makroname */
	char *desc;	/* Makrobeschreibung */
	char *fmt;	/* Makroformat */
} DocMac_t;

void DocMac_clean (DocMac_t *mac);
void DocMac_print (io_t *io, DocMac_t *mac, int mode);


/*	Variablen- und Makrotabellen
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Tabellenname */
	VarTab_t *var;	/* Variablentabelle */
	vecbuf_t mtab;	/* Makrotabelle */
} DocTab_t;

extern reftype_t DocTab_reftype;
DocTab_t *DocTab (const char *name);
void DocTab_setmac (DocTab_t *tab, char *name, char *desc, char *fmt);
DocMac_t *DocTab_getmac (DocTab_t *tab, const char *name);

void DocTab_def (DocTab_t *tab, io_t *in, strbuf_t *buf);
void DocTab_load (DocTab_t *tab, io_t *io);
void DocTab_fload (DocTab_t *tab, const char *name);


/*	Abfragefunktionen
*/

void DocMacInfo (const char *name, const char *desc);

DocTab_t *Doc_gettab (Doc_t *doc, const char *name);
DocMac_t *Doc_getmac (Doc_t *doc, const char *name);
VarTab_t *Doc_vartab (Doc_t *doc);
Obj_t *Doc_getvar (Doc_t *doc, const char *name);
void Doc_pushvar (Doc_t *doc);
void Doc_popvar (Doc_t *doc);
void Doc_cmd (Doc_t *doc, io_t *in);
void Doc_mac (Doc_t *doc, io_t *in);

/*	Initialisierung
*/

extern DocTab_t *GlobalDocTab;
extern Type_t Type_Doc;
extern char *DocPath;
extern char *DocName;

char *ListArg_str (ObjList_t *list, int n);
int ListArg_int (ObjList_t *list, int n);
void DocFunc_section (Func_t *func, void *rval, void **arg);
void SetupDoc (void);

#endif	/* efeudoc.h */
