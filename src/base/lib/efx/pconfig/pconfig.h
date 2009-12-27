/*	Programmkonfiguration
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_PCONFIG_T
#define	EFEU_PCONFIG_T	1

#include <EFEU/Info.h>
#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>

/*	Programmparameter
*/

typedef struct {
	int flag;	/* Steuerflag */
	char *name;	/* Argumentname */
	char *cmd;	/* Rückruffunktion */
	char *desc;	/* Beschreibungsformat */
} pardef_t;

#define	P_INIT		1	/* Initialisierungsstring */
#define	P_ENV		2	/* Environmentparameter */
#define	P_OPT		3	/* Optionen */
#define	P_XOPT		4	/* Spezialoptionen */
#define	P_ARG		5	/* Argumente */
#define	P_XARG		6	/* Spezialargumente */
#define	P_OPTARG	7	/* Optionale Argumente */
#define	P_BREAK		8	/* Abbruch der Argumentanalyse */
#define	P_SET		9	/* Variable initialisieren */


/*	Globale Programmparameter
*/

typedef struct {
	char *init;	/* Konfigurationsdatei */
	vecbuf_t env;	/* Environmentstack */
	vecbuf_t opt;	/* Optionsstack */
	vecbuf_t arg;	/* Argumentstack */
	vecbuf_t xopt;	/* Spezialoptionen */
	vecbuf_t xarg;	/* Spezialargumente */
} deftab_t;

extern deftab_t PgmDefTab;


/*	Initialisierungsfunktionen
*/

void SetupReadline (void);
void SetupInteract (void);
void SetupDebug (void);

io_t *io_readline (const char *prompt, const char *hist);
io_t *io_interact (const char *prompt, const char *hist);

extern io_t *(*_interact_open) (const char *prompt, const char *hist);
extern io_t *(*_interact_filter) (io_t *io);

extern int iorl_key;
extern int iorl_maxhist;

/*	Programmkonfiguration
*/

typedef struct {
	char *name;	/* Programmname */
	int (*eval) (int argc, char **argv);
} pconfig_t;


/*	Initialisierungsfunktionen
*/

void libinit (const char *name);
void pconfig_init (void);
void pconfig_exit (void);
void pconfig (const char *name, Var_t *var, size_t dim);
void loadarg (int *narg, char **arg);
void usage (const char *fmt, io_t *out);
void cp_usage(io_t *in, io_t *out);

#define	PROCINFONAME	"self"

extern InfoNode_t *ProcInfo;
void ArgInfo (io_t *io, InfoNode_t *info);

#define	APPL_APP	1
#define	APPL_HLP	2
#define	APPL_TRM	3
#define	APPL_CFG	4

int applfile (const char *name, int type);
io_t *io_applfile (const char *name, int type);


/*	Globale Variablen
*/

extern char *ProgIdent;
extern char *ProgName;
extern char *ApplPath;
extern char *HelpFmt;
extern char *UsageFmt;

extern char *Shell;
extern char *Pager;
extern char *PS1;
extern char *PS2;

extern int float_align;

/*	Systemaufrufe
*/

int callproc (const char *cmd);

extern xtab_t Buildin;
extern int ExecFlag;
extern int TraceFlag;

/*	Hilfsprogramme zur Argumentzerlegung
*/

double sepfactor (const char *p, char **ptr);
char *sepname (const char *p, char **ptr);
char *ExpandPath (const char *name);

/*	Interpreterprogramme
*/

int EshConfig (int *narg, char **arg);

#endif	/* EFEU_PCONFIG_T */
