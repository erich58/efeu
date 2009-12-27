/*	Ausgabe einer Meldung
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>
#include <EFEU/msgtab.h>
#include <EFEU/parsub.h>
#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/pconfig.h>

#define	ERRFMT	getmsg(MSG_EFEU, 2, "$!: ERROR $1.\n")


/*	Bibliotheksmeldung ausgeben
*/

void errmsg(const char *name, int num)
{
	char *fmt;

	fmt = getmsg(name, num, NULL);

	if	(fmt == NULL)
	{
		if	(name)
		{
			reg_fmt(1, "%s:%d", name, num);
		}
		else	reg_fmt(1, "%d", num);

		fmt = ERRFMT;
	}

	io_psub(ioerr, fmt);
}


void liberror(const char *name, int num)
{
	errmsg(name, num);
	libexit(EXIT_FAILURE);
}
