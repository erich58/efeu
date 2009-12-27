/*	Prozessumgebung
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6

$Header	<EFEU/$1>
*/

#ifndef	EFEU_PROCENV_H
#define	EFEU_PROCENV_H	1

#include <EFEU/config.h>

#ifndef	EXIT_SUCCESS
#define	EXIT_SUCCESS	0
#endif

#ifndef	EXIT_FAILURE
#define	EXIT_FAILURE	1
#endif

extern char *ProgName;		/* Programmname */
extern char *ProgIdent;		/* Programmidentifikation */
extern char *ApplPath;		/* Suchpfad für Hilfsdateien */
extern char *Pager;		/* Seitenfilter */
extern char *Shell;		/* Shell */

#define	MSG_EFM	"efm"

/*	Prozeßinitialisierung
*/

void procinit (const char *name);
void procexit (int status);

extern void (*_procexit_cleanup) (void);


/*	Fehlermeldungen
*/

extern char *MessageHandler;		/* Meldungsverwalter */

void message (const char *id, const char *name, int num, int narg, ...);
void vmessage (const char *id, const char *name, int num, int narg,
	va_list list);

#endif	/* EFEU/procenv.h */
