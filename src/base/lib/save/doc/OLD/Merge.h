/*	EIS-Handbuchformatierung
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.6
*/

#ifndef	EFEU_Document_h
#define	EFEU_Document_h	1

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/vecbuf.h>
#include <EFEU/Info.h>
#include <EFEU/stack.h>
#include <EFEU/DocOut.h>

#ifndef	MSG_DOC
#define	MSG_DOC	"Doc"
#endif


/*	Mischstruktur
*/

#define	HMODE_LINE	1	/* Anfang einer Zeile */
#define	HMODE_WORD	2	/* Anfang eines Wortes */
#define	HMODE_TEXT	3	/* Innerhalb eines Wortes */

typedef struct {
	REFVAR;		/* Referenzvariablen */
	io_t *input;	/* Eingabefile */
	DocOut_t *out;	/* Ausgabe */
	strbuf_t *buf;	/* Buffer für Kommentare */
	int obbeyspace;	/* Alle Leerzeichen berücksichtigen */
	int obbeyline;	/* Alle Zeilenumbrüche berücksichtigen */
	int hmode;	/* Horizontaler Modus */
} Merge_t;

extern reftype_t Merge_reftype;

#define	Merge_close(m)	rd_deref(m)

Merge_t *Merge_open (DocOut_t *out);

void Merge_eval (Merge_t *merge);
void Merge_ioeval (Merge_t *merge, io_t *input);
void Merge_streval (Merge_t *merge, const char *str);

int Merge_getc (Merge_t *merge);
int Merge_xgetc (Merge_t *merge);
int Merge_putc (int c, Merge_t *merge);
int Merge_xputc (int c, Merge_t *merge);

io_t *Merge_io (Merge_t *merge, int (*get) (Merge_t *merge),
	int (*put) (int c, Merge_t *merge));

int Merge_stdcmd (Merge_t *merge, const char *name);
void Merge_expr (Merge_t *merge, int c);
void Merge_tmpcpy (Merge_t *merge, io_t *tmp);
void Merge_cmdeval (Merge_t *merge, io_t *io, const char *delim);
int Merge_expand (Merge_t *merge, int c);
void Merge_printf (Merge_t *merge, const char *fmt, ...);

int Merge_copy (Merge_t *merge, io_t *io, int beg, int end, int flags);
char *Merge_arg (Merge_t *merge, int begkey, int endkey, int flag);
char *Merge_cmdname (Merge_t *merge, int c);
int Merge_ignorespace (Merge_t *merge);

/*	Parse-Funktionen
*/

char *DocParse_name (io_t *io, int c);
char *DocParse_line (io_t *io, int flag);
char *DocParse_arg (io_t *io, int beg, int end);

/*	Makroparameter
*/

typedef vecbuf_t DocMacPar_t;

void DocMacPar_init (DocMacPar_t *mp, const char *name);
void DocMacPar_load (DocMacPar_t *mp, Merge_t *merge, const char *pdef);
void DocMacPar_sub (DocMacPar_t *mp, const char *macdef, io_t *io);
char *DocMacPar_get (DocMacPar_t *mp, int n);
char *DocMacPar_extract (DocMacPar_t *mp, int n);
void DocMacPar_clean (DocMacPar_t *mp);


/*	Dokumentmakros
*/

typedef struct DocMac_s DocMac_t;
typedef void (*DocMacEval_t) (DocMac_t *cmd, Merge_t *merge, DocOut_t *out);
typedef void (*DocMacInfo_t) (DocMac_t *cmd, io_t *out);

struct DocMac_s {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Befehlsname */
	DocMacEval_t eval;	/* Auswertungsfunktion */
	DocMacInfo_t info;	/* Beschreibungsfunktion */
	admin_t admin;	/* Parameteradministrations */
	void *par;	/* Befehlsparameter */
};

extern reftype_t DocMac_reftype;

#define	DocMac_data(name, eval, info, admin, par)	\
{ REFDATA(&DocMac_reftype), name, eval, info, admin, par }

DocMac_t *DocMac (char *name, DocMacEval_t eval,
	DocMacInfo_t info, admin_t admin, void *par);

extern void MergeInfo_Label (DocMac_t *cmd, io_t *io);

extern DocMac_t DocMac_def;
extern DocMac_t DocMac_edef;
extern DocMac_t DocMac_newcmd;
extern DocMac_t DocMac_cmdeval;


/*	Befehlstabellen
*/

typedef struct {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Tabellenname */
	vecbuf_t tab;	/* Tabellendefinition */
} DocMacTab_t;

extern reftype_t DocMacTab_reftype;
DocMacTab_t *DocMacTab (const char *name, size_t size);
void DocMacTab_insert (DocMacTab_t *tab, DocMac_t *cmd);
void DocMacTab_add (DocMacTab_t *tab, DocMac_t *cmd, size_t dim);
DocMac_t *DocMacTab_search (DocMacTab_t *tab, const char *name);
void DocMacTab_print (DocMacTab_t *tab, io_t *io);

void DocMacTab_macro (DocMacTab_t *tab, char *name, char *desc,
	char *macop, char *macdef);
void DocMacTab_cmdeval (DocMacTab_t *tab, char *name, char *desc,
	char *macop, char *macdef);

DocMacTab_t *DocMacGlobal (void);

extern stack_t *DocMacStack;

DocMacTab_t *DocMacStack_tab (const char *name);
DocMac_t *DocMacStack_cmd (const char *tname, const char *name);

/*	Dokumentstruktur
*/

#define	DOC_PARSUB	0x1
#define	DOC_EXPAND	0x2

typedef struct {
	REFVAR;		/* Referenzvariablen */
	VarTab_t *var;	/* Variablentabelle */
	stack_t *stack;	/* Makrostack */
	int obbeyspace;	/* Alle Leerzeichen berücksichtigen */
	int obbeyline;	/* Alle Zeilenumbrüche berücksichtigen */
	int hmode;	/* Horizontaler Modus */
	int lock;	/* Sperrflags */
} Document_t;

extern reftype_t Document_reftype;
extern Type_t Type_Document;

Document_t *Document (const char *name);
void SetupDocument (void);

DocMacTab_t *Document_mactab (Document_t *doc, const char *name);
DocMac_t *Document_macdef (Document_t *doc, const char *tab, const char *name);
void Document_copy (Document_t *doc, io_t *in, DocOut_t *out);


/*	Initialisierung
*/

extern void SetupMerge (void);

extern void MergeEval_init (Merge_t *merge);
extern void MergeEval_exit (Merge_t *merge);

#endif	/* EFEU_Document_h */
