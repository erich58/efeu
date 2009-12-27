/*	Systemaufruf
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/pconfig.h>

void (*callproc_init) (void);
void (*callproc_exit) (void);

int callproc(const char *cmd)
{
	extern char *Shell;
	int n;

	if	(callproc_init)
		callproc_init();

	if	(cmd != NULL)	n = system(cmd);
	else if	(Shell != NULL)	n = system(Shell);
	else			n = -1;

	if	(callproc_exit)
		callproc_exit();

	return n;
}
