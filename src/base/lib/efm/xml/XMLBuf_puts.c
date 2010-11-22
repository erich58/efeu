#include <EFEU/xml.h>
#include <ctype.h>

int XMLBuf_puts (XMLBuf *xml, const char *pfx, const char *str)
{
	int space;
	int quote;

	if	(!xml || !str)	return 0;

	while ((*str && isspace(*str)))
		str++;

	if	(!*str)
		return 0;

	sb_puts(pfx, &xml->sbuf);

	if	(*str == '<')
	{
		sb_putc('\t', &xml->sbuf);
		sb_putc(*str, &xml->sbuf);
		str++;
	}

	for (space = 0, quote = 0; *str; str++)
	{
		if	(*str == quote)
		{
			quote = 0;
		}
		else if	(quote)
		{
			;
		}
		else if	(*str == '"' || *str == '\'')
		{
			quote = *str;
		}
		else if	(isspace(*str))
		{
			if	(!space)
				space = ' ';

			continue;
		}

		if	(*str == '<')
		{
			sb_putc('\n', &xml->sbuf);
			sb_putc('\t', &xml->sbuf);
			space = 0;
		}
		else if	(space)
		{
			sb_putc(space, &xml->sbuf);
			space = 0;
		}

		sb_putc(*str, &xml->sbuf);

		if	(*str == '>')
			space = '\n';
	}

	return 1;
}
