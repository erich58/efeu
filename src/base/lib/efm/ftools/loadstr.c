/*	String einlesen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.6
*/


#include <EFEU/memalloc.h>
#include <EFEU/ftools.h>

char *loadstr (FILE *file)
{
	register size_t size;
	register char *str;
	
	if	((size = get2byte(file)) == 0)
		return NULL;

	str = memalloc(size);

	if	(fread(str, 1, size, file) != size)
	{
		memfree(str);
		fprintf(stderr, "loadstr: Unerwartetes Fileende.\n");
		exit(1);
	}

	return str;
}
