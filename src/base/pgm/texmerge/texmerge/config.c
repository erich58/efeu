/*	Konfigurationsdatei öffnen
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <ctype.h>


/*	Eingabestruktur öffnen
*/

Input_t *OpenConfig(const char *name)
{
	char *path;
	char *fname;
	FILE *file;
	Buffer_t *buf;

	if	(name == NULL)	name = CFG_NAME;

	if	(name[0] != '.' && name[0] != DIRSEP)
	{
		path = IncPath;
	}
	else	path = NULL;

	while (path && *path)
	{
		buf = CreateBuffer();

		for (; *path && *path != *PATHSEP; path++)
			buf_putc(*path, buf);

		buf_putc(DIRSEP, buf);
		buf_puts(name, buf);
		buf_putc('.', buf);
		buf_puts(CFG_EXT, buf);

		fname = CloseBuffer(buf);
		file = fopen(fname, "r");

		if	(file)
			return CreateInput(fname, file, NULL);

		FreeData("STR", fname);

		if	(*path == *PATHSEP)	path++;
	}

	buf = CreateBuffer();
	buf_puts(name, buf);
	buf_putc('.', buf);
	buf_puts(CFG_EXT, buf);
	fname = CloseBuffer(buf);
	file = fopen(fname, "r");

	if	(file == NULL)
		Error(10, name);

	return CreateInput(fname, file, NULL);
}


void LoadConfig(const char *fname)
{
	Input_t *in;
	Output_t *out;

	in = OpenConfig(fname);
	out = OpenDummy();

#if	UNIX_VERSION
	AddDepend(in->name);
#else
	if	(MakeDepend && in->name)
		printf(" %s", in->name);
#endif

	if	(VerboseMode && in->name)
		fprintf(stderr, "Laden von %s.\n", in->name);
		
	Merge(in, out);
	CloseInput(in);
	CloseOutput(out);
}
