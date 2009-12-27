/*	Parameter eines Kommandos
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#ifndef	EFEU_CmdPar_h
#define	EFEU_CmdPar_h	1

#include <EFEU/config.h>

#define	CmdPar_Environ	0x1	/* Umgebungsvariable */
#define	CmdPar_Option	0x2	/* Option */
#define	CmdPar_Argument	0x3	/* Argument */
#define	CmdPar_Pattern	0x4	/* Muster */
#define	CmdPar_VaList	0x5	/* Variable Argumentliste */
#define	CmdPar_Mask	0xf	/* Maske für Parametertype */

#define	CmdParArg_noarg		0	/* Kein Argument */
#define	CmdParArg_required	1	/* notwendiges Argument */
#define	CmdParArg_optional	2	/* optionales Argument */

/*	Programmresourcen
*/

typedef struct {
	void (*set) (const char *name, void *ptr, char *arg);
	char *(*get) (const char *name, void *ptr);
} CmdResHandler_t;

extern CmdResHandler_t *CRH_int;
extern CmdResHandler_t *CRH_dbl;
extern CmdResHandler_t *CRH_str;

typedef struct {
	const char *name;
	CmdResHandler_t *handler;
	void *ptr;
	const char *defval;
} CmdRes_t;

void AddCmdRes (CmdRes_t *res, size_t dim);

/*	Programmparameter
*/

void AddCmdParDef (const char *def);
void LoadCmdParDef (const char *name);
void ParseCmdLine (int *argc, char **argv);

#endif	/* EFEU_CommandArg_h */
