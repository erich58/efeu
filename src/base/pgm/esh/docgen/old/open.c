/*	Eingabestruktur öffnen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"


io_t *OpenInput (const char *path, const char *name, const char *ext)
{
	char *fname;

	if	(name == NULL)	return NULL;

	fname = IncPath ? fsearch(IncPath, NULL, name, ext) : NULL;

	if	(fname == NULL)
		fname = fsearch(NULL, NULL, name, ext);

	if	(fname)
	{
		io_t *io = io_fileopen(fname, "rz");

		if	(VerboseMode)
		{
			reg_set(1, fname);
			io_psub(ioerr, "Laden von $1.\n");
		}
		else	memfree(fname);

		return io_lnum(io);
	}

	reg_fmt(1, "%#s", name);
	errmsg(NULL, 10);
	return NULL;
}
