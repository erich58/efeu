/*	Infotext aus Datei laden
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>

#define	BUFSIZE	4096

static char buf[BUFSIZE];


static void FileLabel_print (io_t *io, FileLabel_t *par)
{
	FILE *file;
	int n;

	if	(par->name == NULL)	return;

	file = fopen(par->name, "r");

	if	(file == NULL)
	{
		io_printf(io, "<%s: %d", par->name, par->start + 1);

		if	(par->end > par->start + 2)
			io_printf(io, " - %d", par->end - 1);

		io_puts(">\n", io);
		return;
	}

	for (n = 1; fgets(buf, BUFSIZE, file); n++)
	{
		if	(n <= par->start)	;
		else if	(n >= par->end)		break;
		else				io_psub(io, buf);
	}

	fclose(file);
}

InfoType_t InfoType_FileLabel = { NULL, (InfoPrint_t) FileLabel_print };
