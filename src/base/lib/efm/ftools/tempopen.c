/*	Temporäre Datei öffnen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5

	Version 0.4
*/

#include <EFEU/ftools.h>
#include <EFEU/procenv.h>


FILE *tempopen (void)
{
	FILE *file;

	if	((file = tmpfile()) == NULL)
	{
		message("tempopen", MSG_FTOOLS, 5, 0);
		procexit(EXIT_FAILURE);
		return NULL;
	}
	
	filenotice(mstrcpy("<temp>"), file, fclose);
	return file;
}

