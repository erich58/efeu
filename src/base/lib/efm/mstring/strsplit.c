/*	Aufspalten eines Strings
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <EFEU/strbuf.h>

#define	SPACE(x)	((x) == ' ' || (x) == '\t')

char *nextstr (char **ptr)
{
	char *p = *ptr;

	while (**ptr != 0)
		(*ptr)++;

	(*ptr)++;
	return (*p == 0 ? NULL : p);
}

size_t strsplit(const char *str, const char *delim, char ***ptr)
{
	strbuf_t *sb;
	size_t i, dim;
	char *p;

	if	(ptr == NULL)	return 0;
	else			*ptr = NULL;

	if	(str == NULL)	return 0;

	sb = new_strbuf(0);
	i = dim = 0;

/*	String umkopieren
*/
	for (; listcmp(delim, *str); str++)
	{
		if	(!SPACE(*str))
		{
			sb_putc(0, sb);
			dim++;
		}
	}

	while (*str != 0)
	{
		if	(listcmp(delim, *str))
		{
			for (; listcmp(delim, *str); str++)
			{
				if	(!SPACE(*str))
				{
					sb_putc(0, sb);
					dim++;
					i = 0;
				}
			}

			if	(i)
			{
				sb_putc(0, sb);
				dim++;
				i = 0;
			}
		}
		else
		{
			sb_putc(*str, sb);
			str++;
			i++;
		}
	}

	if	(i)
	{
		sb_putc(0, sb);
		dim++;
	}

/*	Initialisieren der Teilstringstruktur
*/
	if	(dim)
	{
		*ptr = (char **) memalloc(sb->pos + dim * sizeof(char *));
		p = (char *) ((*ptr) + dim);
		memcpy(p, sb->data, sb->pos);

		for (i = 0; i < dim; i++, p++)
			for ((*ptr)[i] = p; *p != 0; p++)
				;
	}

	del_strbuf(sb);
	return dim;
}
