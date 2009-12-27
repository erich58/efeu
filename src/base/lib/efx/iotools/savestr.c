/*	String zwischenspeichern
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/efmain.h>
#include <EFEU/efio.h>


size_t io_savestr(io_t *tmp, const char *str)
{
	size_t n;

	if	(str == NULL)
	{
		io_putc(0, tmp);
		return 0;
	}

	io_putc(1, tmp);
	n = io_puts(str, tmp);
	io_putc(0, tmp);
	return n + 1;
}


char *io_loadstr(io_t *tmp, char **ptr)
{
	char *p;
	int c;

	if	((c = io_getc(tmp)) <= 0)
	{
		return NULL;
	}

	p = *ptr;

	do
	{
		c = io_getc(tmp);
		**ptr = (char) c;
		(*ptr)++;
	}
	while	(c > 0);

	return p;
}
