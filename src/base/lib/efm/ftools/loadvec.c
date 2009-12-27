/*	Vektorbuffer aus Datei laden
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/

#include <EFEU/vecbuf.h>
#include <EFEU/ftools.h>
#include <EFEU/procenv.h>

size_t loadvec (FILE *file, void *ptr, size_t size, size_t dim)
{
	size_t n = fread_compat(ptr, size, dim, file);

	if	(n != dim)
		fileerror(file, MSG_FTOOLS, 21, 0);

	return n;
}

size_t savevec (FILE *file, const void *ptr, size_t size, size_t dim)
{
	size_t n = fwrite_compat(ptr, size, dim, file);

	if	(n != dim)
		fileerror(file, MSG_FTOOLS, 22, 0);

	return n;
}
