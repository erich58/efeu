/*	Datei suchen und zum Lesen �ffnen
	(c) 1999 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>


FILE *findopen (const char *path, const char *pfx,
	const char *name, const char *ext, const char *mode)
{
	char *fname = fsearch(path, pfx, name, ext);

	if	(fname != NULL)
	{
		FILE *file = fileopen(fname, mode);
		memfree(fname);
		return file;
	}

	fname = mstrpaste(".", name ? name : "", ext);
	message("findopen", MSG_FTOOLS, 6, 1, fname);
	memfree(fname);
	procexit(EXIT_FAILURE);
	return NULL;
}
