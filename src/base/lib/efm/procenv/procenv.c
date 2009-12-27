/*	Prozesskontrolle
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/procenv.h>
#include <EFEU/mstring.h>
#include <EFEU/ftools.h>

/*	Systemabhängige Parameter:

S_SHELLENV	Environmentvariable mit Befehlsinterpreter.
S_SYSNAME	Systemname
S_APPLPATH	Standardsuchpfad
S_PGM_SHELL	Standardbefehlsinterpreter
S_PGM_PAGER	Standardseitenfilter
*/

#define S_APPLPATH	".:/efeu/lib/efeu/%L/%S:/efeu/lib/efeu/%S:~/lib/%S"
#define S_PGM_SHELL	"/bin/sh"
#define S_PGM_PAGER	"|less -r"

char *ProgIdent = NULL;
char *ProgName = NULL;
char *ApplPath = S_APPLPATH;
char *Shell = S_PGM_SHELL;
char *Pager = S_PGM_PAGER;


void procinit (const char *name)
{
	fname_t *pgname;

	if	((pgname = strtofn(name)) != NULL)
	{
		memfree(ProgName);
		memfree(ProgIdent);
		ProgName = mstrcpy(pgname->name);
		ProgIdent = mstrcpy(ProgName);
		memfree(pgname);
	}
}

void (*_procexit_cleanup) (void) = NULL;

void procexit (int status)
{
	if	(_procexit_cleanup)
		_procexit_cleanup();

	closeall();
	exit(status);
}
