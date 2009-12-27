/*	Einmischen von Daten in TeX-Dateien
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#ifndef	EISDOC_H
#define	EISDOC_H	1

/*	Konfiguration
*/

#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/mdmat.h>
#include <Math/TimeSeries.h>

/*	Makros und globale Variablen
*/

#define	CFG_NAME	"eisdoc"
#define	CFG_EXT		"cfg"

extern char *IncPath;
extern int VerboseMode;
extern int MakeDepend;

/*	Ausgabefilter
*/

#define	CHARSET(n)	((n) << 8)

#define	CHAR_MINUS	(CHARSET(1) | '-')
#define	CHAR_DASH	(CHARSET(2) | '-')
#define	CHAR_SPACE	(CHARSET(1) | ' ')
#define	CHAR_GLQQ	(CHARSET(1) | '"')
#define	CHAR_GRQQ	(CHARSET(2) | '"')
#define	CHAR_FLQQ	(CHARSET(1) | '<')
#define	CHAR_FRQQ	(CHARSET(1) | '>')

typedef void (*FilterPut_t) (int c, io_t *io);

typedef struct {
	char *name;
	char *desc;
	FilterPut_t put;
} Filter_t;

Filter_t *GetFilter (const char *name);


/*	Mischstruktur
*/

typedef void (*EvalCmd_t) (const char *name);

typedef struct {
	io_t *in;		/* Eingabestruktur */
	io_t *out;		/* Ausgabestruktur */
	char cmdkey;		/* Befehlskennung */
	char comkey;		/* Kommentarkennung */
	int linestart : 4;	/* Flag für Zeilenanfang */
	int ignorespace : 4;	/* Leerzeichen am Zeilenanfang entfernen */
	int keepcom : 4;	/* Kommentare erhalten */
	int outchar : 4;	/* Ausgegebene Zeichen */
	int pos : 32;		/* Ausgabeposition (für Tabulatoren) */
	Filter_t *filter;	/* Ausgabefilter */
	EvalCmd_t eval;		/* Mischbefehle ausführen */
	strbuf_t *buf;		/* Buffer für Kommentare */
} Merge_t;

#define	MERGE_DATA(cmd, com)	\
{ NULL, NULL, cmd, com, 0, 0, 0, 0, 0, NULL, NULL, NULL }

extern Merge_t MergeStat;
extern io_t *MergeOut;
extern io_t *MergeCtrlOut;

void PushMerge (Merge_t *save);
void PopMerge (Merge_t *save);

void MergeInput (io_t *in);
void MergeOutput (io_t *out);
void MergeEval (int beg, int end);
int EvalMergeCmd(int c);
char *MergeComment (void);

void MergeCtrl (int c);
void MergePutc (int c);
void MergePuts (const char *str);
void MergePutCmd (const char *str);
void MergeNewline (void);
void MergeFilter (const char *name);


/*	Befehlstabellen
*/

typedef struct Cmd_s Cmd_t;

extern reftype_t CmdTabType;

typedef struct CmdTab_s CmdTab_t;

struct CmdTab_s {
	REFVAR;		/* Referenzvariablen */
	char *name;	/* Name der Tabelle */
	vecbuf_t buf;	/* Buffer mit Befehlen */
};

#define	CMDTABDATA(label)	\
{ REFDATA(&CmdTabType), label, VB_DATA(16, sizeof(Cmd_t)) }

extern CmdTab_t *builtin;
extern CmdTab_t *globtab;

CmdTab_t *NewCmdTab (const char *name);
void ListCmdTab (io_t *io, CmdTab_t *tab);
void PushCmdTab (CmdTab_t *tab);
CmdTab_t *PopCmdTab (void);


/*	Hilfsprogramme
*/

#define	FileName(name, ext)	mstrpaste(".", name, ext)

void LoadConfig (const char *name);
void StringConfig (const char *str);

io_t *OpenInput (const char *path, const char *name, const char *ext);
io_t *OpenInteractive (void);

void FormatString (const char *str, io_t *out, const char *fmt);
void out_newline (io_t *out);
int set_mode(io_t *io, int mode);

void tex_putc (int c, io_t *out);
void tex_puts (const char *str, io_t *out);

io_t *OpenTexfile (const char *name);


/*	Argumentlisten
*/

typedef struct {
	int size;
	int dim;
	char **arg;
	char *opt;
} ArgList_t;

void InitArgList (ArgList_t *list);
void ClearArgList (ArgList_t *list);

void AddOpt (ArgList_t *list, char *arg);
void AddArg (ArgList_t *list, char *arg);
void LoadArg (ArgList_t *list, int narg);
int SyncArg (ArgList_t *list, int narg);
int LongArg (const char *fmt);


/*	Mischbefehle
*/

typedef void (*CmdEval_t) (Cmd_t *cmd, ArgList_t *arg);
typedef void (*CmdShow_t) (Cmd_t *cmd, io_t *io);

typedef struct {
	char *name;	/* Bezeichnung des Types */
	clean_t clean;	/* Aufräumfunktion für Parameter */
	CmdShow_t show;	/* Darstellungsfunktion */
	CmdEval_t eval;	/* Auswertungsfunktion */
} CmdType_t;

struct Cmd_s {
	char *name;
	char *desc;
	void *par;
	int narg;
	CmdType_t *type;
};

Cmd_t *GetCmd (CmdTab_t *tab, const char *name);
Cmd_t *AddCmd (CmdTab_t *tab, const char *name);
void DelCmd (CmdTab_t *tab, const char *name);

void SetMacro (Cmd_t *cmd, const char *type, int narg, const char *fmt);
void ShowMacro (Cmd_t *cmd, io_t *io);

Cmd_t *AddMergeCmd (const char *name);
Cmd_t *GetMergeCmd (const char *name);
void DelMergeCmd (const char *name);
void ListMergeCmd (io_t *io);


/*	Hilfsfunktionen zur Ein/Ausgabe
*/

void SkipLine (void);
int SkipComment (int c);

char *ParseName (void);
int ParseArg (char **ptr, int beg, int end);

/*	Standardauswertungsfunktionen
*/

void ce_def (Cmd_t *cmd, ArgList_t *arg);
void ce_let (Cmd_t *cmd, ArgList_t *arg);
void ce_undef (Cmd_t *cmd, ArgList_t *arg);

void ce_macro (Cmd_t *cmd, ArgList_t *arg);
void ce_var (Cmd_t *cmd, ArgList_t *arg);
void ce_expr (Cmd_t *cmd, ArgList_t *arg);
void ce_opt (Cmd_t *cmd, ArgList_t *arg);
void ce_arg (Cmd_t *cmd, ArgList_t *arg);
void ce_show (Cmd_t *cmd, ArgList_t *arg);

void ce_eval (Cmd_t *cmd, ArgList_t *arg);
void ce_begin (Cmd_t *cmd, ArgList_t *arg);
void ce_if (Cmd_t *cmd, ArgList_t *arg);

void BuiltinCommand (void);

void TexMerge (const char *name, const char *out);
void EvalMacro (const char *macro, io_t *out);
void MergeShow (const char *cmd, io_t *output);

Obj_t *cpar_name (void *par, const ObjList_t *list);
Obj_t *cpar_opt (void *par, const ObjList_t *list);
Obj_t *cpar_narg (void *par, const ObjList_t *list);
Obj_t *cpar_arg (void *par, const ObjList_t *list);

io_t *OutputFilter (io_t *io);

#define	OUT_CTRL	('o' << 8)
#define	OUT_NEWLINE	(OUT_CTRL | 0x01)	/* Neue Zeile beginnen */
#define	OUT_FILTER	(OUT_CTRL | 0x02)	/* Ausgabefilter definieren */
#define	OUT_MODE	(OUT_CTRL | 0x03)	/* Ausgabemodus setzen */

#define	OMODE_EXPAND	0x1	/* Tabulatoren Expandieren */
#define	OMODE_FILTER	0x2	/* Filter verwenden */

#endif	/* EISDOC_H */
