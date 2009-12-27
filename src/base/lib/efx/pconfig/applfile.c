/*	Konfigurationsdatei öffnen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/pconfig.h>
#include <EFEU/parsedef.h>


io_t *io_applfile(const char *name, int type)
{
	char *p, *pfx, *sfx;
	io_t *io;

	switch (type)
	{
	case APPL_APP:	pfx = "app-defaults"; sfx = "app"; break;
	case APPL_CFG:	pfx = "app-defaults"; sfx = "cfg"; break;
	case APPL_TRM:	pfx = "term"; sfx = "trm"; break;
	default:	pfx = NULL; sfx = NULL; break;
	}

	p = fsearch(ApplPath, pfx, name, sfx);
	io = p ? io_fileopen(p, "r") : NULL;
	FREE(p);
	return io;
}


int applfile(const char *name, int type)
{
	io_t *io = io_applfile(name, type);

	if	(io != NULL)
	{
		io = io_lnum(io);
		io_eval(io, NULL);
		io_close(io);
		return 1;
	}
	else	return 0;
}
