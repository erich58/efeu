/*	Einmischen von Daten in TeX-Dateien
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#ifndef	TEXMERGE_H
#define	TEXMERGE_H	1

/*	Konfiguration
*/

#ifndef	DOS_VERSION
#define	DOS_VERSION	0
#endif

#if	DOS_VERSION
#define	UNIX_VERSION	0
#else
#define	UNIX_VERSION	1
#endif

#ifndef	PATHSEP
#define	PATHSEP	":"
#endif

#ifndef	DIRSEP
#define	DIRSEP	'/'
#endif

#if	UNIX_VERSION

#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>
#include <EFEU/preproc.h>
#include <EFEU/cmdeval.h>
#include <EFEU/efutil.h>
#include <EFEU/database.h>
#include <EFEU/mdmat.h>
#include <EFEU/Random.h>
#include <EFEU/MakeDepend.h>
#include <Math/TimeSeries.h>

#else

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#endif

/*	Makros und globale Variablen
*/

#define	TPL_EXT		"tpl"
#define	TEX_EXT		"tex"
#define	CFG_ENV		"TEXMERGE"
#define	CFG_NAME	"texmerge"
#define	CFG_EXT		"cfg"

extern char *IncPath;
extern char *ProgramName;
extern int VerboseMode;
extern int MakeDepend;
extern int MergeMode;


/*	Hilfsprogramme
*/

extern int alloc_debug;

void *AllocData (const char *name, size_t size);
void FreeData (const char *name, void *data);
char *PasteString (const char *delim, const char *s1, const char *s2);

#define	FileName(name, ext)	PasteString(".", name, ext)
#define	CopyString(str)		PasteString(NULL, str, NULL)

void LoadConfig (const char *name);
void StringConfig (const char *str);
void PrintError (int num, va_list args);
void Error (int num, ...);


/*	Zeichenbuffer
*/

typedef struct {
	char *data;	/* Datenbuffer */
	size_t size;	/* Aktuelle Größe des Buffers */
	size_t pos;	/* Aktuelle Bufferposition */
} Buffer_t;

Buffer_t *CreateBuffer (void);
char *CloseBuffer (Buffer_t *buf);

void buf_putc (int c, Buffer_t *buf);
void buf_puts (const char *str, Buffer_t *buf);


/*	Eingabestruktur
*/

typedef struct Input_s Input_t;

struct Input_s {
	char *name;
	int line;
	int save;
	char nl;
	char eof;
	int (*get) (Input_t *input);
	void (*close) (Input_t *input);
	void (*ident) (Input_t *input);
	void *data;
};

Input_t *CreateInput (char *name, FILE *file, const char *str);
void CloseInput (Input_t *in);
void InputError (Input_t *in, int num, ...);

int in_getc (Input_t *input);
void in_ungetc (int c, Input_t *input);
void in_skipline (Input_t *input);

Input_t *OpenTemplate (const char *name);
Input_t *OpenConfig (const char *name);
Input_t *OpenInteractive (void);

/*	Ausgabestruktur
*/

typedef struct Output_s Output_t;

#define	OUT_TEXMODE	0x1	/* TeX-kompatible Darstellung */
#define	OUT_STRMODE	0x2	/* Stringformat */
#define	OUT_EXPAND	0x4	/* Tabulatoren expandieren */

struct Output_s {
	char *name;	/* Filename */
	int mode;	/* Ausgabemodus */
	int pos;	/* Aktuelle Position */
	void (*put) (int c, Output_t *out);
	void (*puts) (const char *str, Output_t *out);
	void (*close) (Output_t *out);
	void *data;	/* Ausgabestruktur */
};

Output_t *CreateOutput (char *name, FILE *file, Buffer_t *buf);
void CloseOutput (Output_t *out);

int set_mode (Output_t *out, int mode);
int out_putc (int c, Output_t *out);
void out_fputs (const char *str, Output_t *out, const char *fmt);
void out_puts (const char *str, Output_t *out);
void out_newline (Output_t *out);

void tex_putc (int c, Output_t *out);
void str_putc (int c, Output_t *out);

Output_t *OpenTexfile (const char *name);
Output_t *OpenDummy (void);
Output_t *OpenStdout (void);

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
int LoadArg (ArgList_t *list, Input_t *in, int narg);
int SyncArg (ArgList_t *list, int narg);
int LongArg (const char *fmt);

/*	Mischbefehle
*/

#define	TM_BUILTIN	'b'	/* Eingebauter Befehl */
#define	TM_CONTROL	'x'	/* Kontrollstruktur */
#define	TM_MACRO	'm'	/* Makrodefinition */
#define	TM_VAR		'v'	/* Variable */
#define	TM_COMMAND	'c'	/* Befehlsausdruck */

typedef struct Command_s Command_t;

typedef void (*CMDEval_t) (Command_t *cmd, Input_t *in, Output_t *out,
	ArgList_t *arg);

struct Command_s {
	char *name;
	char *fmt;
	int type;
	int narg;
	char *desc;
	CMDEval_t eval;
};

char *CommandType(Command_t *cmd);
void AddCommand (char *name, int type, int narg, char *fmt);
void AddSpecial (char *name, CMDEval_t eval, int narg, char *fmt, char *desc);
void AddControl (char *name, CMDEval_t eval, int narg, char *fmt, char *desc);
void DeleteCommand (const char *name);
void CopyCommand (char *name, const char *old);
void AssignCommand (char *name, char *def);
Command_t *SearchCommand (const char *name);

void SkipLine (Input_t *in);
int SkipSpace (Input_t *in);
char *GetName (Input_t *in, int c);

char *GetArg (Input_t *in, int start, int end);
char *GetStdArg (Input_t *in);
char *GetOptArg (Input_t *in);

extern int GetArgStatus;


/*	Standardauswertungsfunktionen
*/

void ce_comment (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_block (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_macro (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_var (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_expr (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_opt (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_arg (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);
void ce_show (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);

void ce_if (Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg);

void BuiltinCommand (void);
void ListCommand (void);

char *GetCommand (Input_t *in, Output_t *out);
void MergeCommand (Input_t *in, Output_t *out, const char *name);
void Merge (Input_t *in, Output_t *out);
void MergeBuf (Input_t *in, Buffer_t *buf);
void TexMerge (const char *name, const char *out);
void MergeEval (const char *cmd, Output_t *out);
void MergeMacro (const char *macro, Output_t *out);
void MergeShow (const char *cmd, Output_t *output);


/*	Versionsabhängige Hilfsprogramme
*/

#if	UNIX_VERSION

io_t *io_Output (Output_t *output);
Input_t *OpenIOInput (io_t *io);

Obj_t *Parse_cout (io_t *io, void *data);
Obj_t *Parse_opt (io_t *io, void *data);
Obj_t *Parse_narg (io_t *io, void *data);
Obj_t *Parse_arg (io_t *io, void *data);

#endif

#endif	/* TEXMERGE_H */
