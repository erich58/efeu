/*	String mit L�ngeninformation ausgeben
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.6
*/


#include <EFEU/memalloc.h>
#include <EFEU/ftools.h>

void savestr (const char *str, FILE *file)
{
	register size_t size;
	
	size = str ? 2 * (strlen(str) / 2) + 2 : 0;
	put2byte(size, file);

	while (size-- > 0)
	{
		register int c = *str ? *str++ : 0;

		if	(putc(c, file) == EOF)
		{
			fprintf(stderr, "savestr: Ausgabefehler.\n");
			exit(1);
		}
	}
}
