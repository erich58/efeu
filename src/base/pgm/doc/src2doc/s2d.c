/*	Verarbeitungsmodi
	(c) 2000 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/patcmp.h>
#include <EFEU/mstring.h>
#include "src2doc.h"

typedef struct {
	char *name;
	char *desc;
	S2DEval_t eval;
} S2DMode_t;

static S2DMode_t tab[] = {
	{ "hdr", "Handbucheintrag für Headerfile", s2d_hdr },
	{ "src", "Handbucheintrag für Sourcefile", s2d_src },
	{ "std", "Sourcen mit Erläuterung", s2d_std },
	{ "com", "Kommentare der Datei", s2d_com },
	{ "doc", "Direkteinbindung der Datei", s2d_doc },
};

typedef struct {
	char *pat;
	S2DEval_t eval;
} S2DName_t;

static S2DName_t ntab[] = {
	{ "*.h", s2d_hdr },
	{ "*.pph", s2d_hdr },
	{ "*.c", s2d_src },
	{ "*.doc", s2d_doc },
	{ "*.[0-9]*", s2d_man },
	{ "*Imakefile", s2d_std },
};

S2DEval_t S2DName_get (const char *name)
{
	int i;

	for (i = 0; i < tabsize(ntab); i++)
		if (patcmp(ntab[i].pat, name, NULL)) return ntab[i].eval;

	return NULL;
}


S2DEval_t S2DMode_get (const char *name)
{
	int i;

	for (i = 0; i < tabsize(tab); i++)
		if (mstrcmp(name, tab[i].name) == 0) return tab[i].eval;

	return NULL;
}

void S2DMode_list (io_t *io)
{
	int i, j, k;

	for (i = 0; i < tabsize(tab); i++)
	{
		io_printf(io, "%s\t%s\n", tab[i].name, tab[i].desc);

		for (j = k = 0; j < tabsize(ntab); j++)
		{
			if	(tab[i].eval == ntab[j].eval)
			{
				io_puts(k ? ", " : "\t", io);
				io_puts(ntab[j].pat, io);
				k++;
			}
		}

		if	(k)	io_putc('\n', io);
	}
}
