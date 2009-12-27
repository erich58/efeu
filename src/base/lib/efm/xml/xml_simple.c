/*
Kompakte, einfach gebaute XML-Dateien
*/

#include <EFEU/xml.h>

#define	TAG_DELIM	'/'

#define LINE_END(c)	((c) == EOF || (c) == '\n')
#define SPACE(c)	((c) == ' '  || (c) == '\t')
#define TAG_END(c)	(LINE_END(c) || (c) == '=' || SPACE(c))
#define TSEP_IN(c)	((c) == '/')
#define TSEP_BUF(c)	((c) == '/' || (c) == 0)

static void out_data (XMLOutput *out, const char *data)
{
	if	(!data)	return;

	while (*data)
		out->put(*data++, out->par);
}

void *xml_simple_print (XMLBuf *xml, const char *name,
	const char *data, void *par)
{
	XMLOutput *out = par;

	if	(!out || !out->put)	return NULL;

	switch (xml->stat)
	{
	case xml_data:
	case xml_entry:
		out_data(out, (char *) xml->sbuf.data);
		out->put('\t', out->par);
		out_data(out, data);
		out->put('\n', out->par);
		break;
	default:
		break;
	}

	return NULL;
}

static void add_line (int32_t c, StrBuf *sb,
	int32_t (*get) (void *par), void *par)
{
	while (SPACE(c))
		c = get(par);

	if	(c == '#')
	{
		while (!LINE_END(c))
			c = get(par);

		return;
	}

	while (!LINE_END(c))
	{
		if	(c == '\\')
		{
			c = get(par);

			switch (c)
			{
			case '\n':
				do	c = get(par);
				while	(SPACE(c));
				
				continue;
			case EOF:
				return;
			case 'n':
				c = '\n';
				break;
			case 't':
				c = '\t';
				break;
			default:
				break;
			}
		}

		sb_putucs(c, sb);
		c = get(par);
	}
}


void *xml_simple_parse (XMLBuf *xml, int32_t (*get) (void *par), void *par)
{
	int32_t c;
	void *res;

	res = NULL;

	while (!res && (c = get(par)) != EOF)
	{
		if	(SPACE(c) || c == '\n')
		{
			;
		}
		else if	(c == '#')
		{
			add_line(c, NULL, get, par);
		}
		else
		{
			int pos;
			int last;

			pos = 0;
			last = 0;

			while (pos < xml->data && !TAG_END(c))
			{
				if	(TSEP_IN(c))
				{
					if	(TSEP_BUF(xml->sbuf.data[pos]))
					{
						last = pos + 1;
					}
					else	break;
				}
				else if	(c != xml->sbuf.data[pos])
				{
					break;
				}

				pos++;
				c = get(par);
			}

			while (xml->depth && last < xml->data)
			{
				if	((res = XMLBuf_end(xml)))
					return res;
			}

			xml->sbuf.pos = pos;
			sb_sync(&xml->sbuf);

			while (!TAG_END(c))
			{
				if	(TSEP_IN(c))
				{
					XMLBuf_next(xml);
					res = XMLBuf_action(xml, xml_beg);

					if	(res)
						return res;
				}
				else	sb_putucs(c, &xml->sbuf);

				c = get(par);
			}

			last = XMLBuf_next(xml);
			add_line(c, &xml->sbuf, get, par);
			res = XMLBuf_action(xml, xml_entry);
			XMLBuf_prev(xml, last);
		}
	}

	return res;
}
