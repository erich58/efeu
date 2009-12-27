/*	Parametersubstitution
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/vecbuf.h>
#include <EFEU/parsub.h>
#include <ctype.h>

int psub_key = '$';

/*	Tabelle mit Substitutionsfunktionen
*/

static VECBUF(psubtab, 32, sizeof(copydef_t));

static int pdef_cmp(copydef_t *a, copydef_t *b)
{
	return (b->key - a->key);
}

void psubfunc(int key, iocopy_t copy, void *par)
{
	copydef_t cdef;

	cdef.key = key;
	cdef.copy = copy;
	cdef.par = par;
	vb_search(&psubtab, &cdef, (comp_t) pdef_cmp, copy ? VB_REPLACE : VB_DELETE);
}


/*	Kopierfunktion
*/

int iocpy_psub(io_t *in, io_t *out, int c, const char *arg, unsigned int flags)
{
	copydef_t def, *ptr;

	def.key = io_getc(in);
	ptr = vb_search(&psubtab, &def, (comp_t) pdef_cmp, VB_SEARCH);

	if	(ptr && ptr->copy)
	{
		return ptr->copy(in, out, ptr->par);
	}
	else if	(isdigit(def.key))
	{
		return io_puts(reg_get(def.key - '0'), out);
	}
	else	return (io_putc(def.key, out) != EOF) ? 1 : 0;
}


int io_pcopy(io_t *in, io_t *out, void *arg)
{
	int c;
	int n;

	n = 0;

	while ((c = io_mgetc(in, 1)) != EOF)
	{
		if	(c == psub_key)
		{
			n += iocpy_psub(in, out, c, NULL, 0);
		}
		else	n += io_nputc(c, out, 1);
	}

	return n;
}

char *mpcopy(io_t *in)
{
	return miocopy(in, io_pcopy, NULL);
}


int io_psub(io_t *out, const char *str)
{
	return iocopystr(str, out, io_pcopy, NULL);
}


char *parsub(const char *str)
{
	return miocopystr(str, io_pcopy, NULL);
}
