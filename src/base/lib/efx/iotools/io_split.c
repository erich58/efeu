/*	Aufspalten einer IO-Struktur in Teilstrings
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>


size_t io_split(io_t *io, const char *delim, char ***ptr)
{
	io_t *tmp;
	ulong_t size;
	size_t i, dim;
	char *p;
	int c;

	if	(ptr == NULL)
	{
		return 0;
	}
	else	*ptr = NULL;

	tmp = io_tmpbuf(0);
	size = 0;
	dim = 0;

/*	Teilstrings zwischenspeichern
*/
	while ((c = io_eat(io, delim)) != EOF)
	{
		p = io_mgets(io, delim);
		size += io_savestr(tmp, p);
		FREE(p);
		dim++;
	}

/*	Stringliste generieren
*/
	*ptr = (char **) memalloc(size + dim * sizeof(char *));
	p = (char *) ((*ptr) + dim);
	io_rewind(tmp);

	for (i = 0; i < dim; i++)
	{
		(*ptr)[i] = io_loadstr(tmp, &p);
	}

	io_close(tmp);
	return dim;
}
