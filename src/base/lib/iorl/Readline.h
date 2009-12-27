/*	Programmkonfiguration
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#ifndef	EFEU_Readline_h
#define	EFEU_Readline_h	1

#include <EFEU/pconfig.h>

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

int EshConfig (int *narg, char **arg);

#endif	/* EFEU_Readline_h */
