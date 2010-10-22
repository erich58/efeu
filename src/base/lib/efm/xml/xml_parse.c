#include <EFEU/xml.h>
#include <ctype.h>

#define	BLANK(c)	((c) == ' ' || c == '\t')
#define	SPACE(c)	(BLANK(c) || c == '\n' || c == '\r')
#define	END_NAME(c)	((c) == '>' || (c) == '/' || BLANK(c))

#define E_EOF   "unexpected end of file"
#define E_CHAR  "unexpected character %#x"

#define	STAT_SKIP	0
#define	STAT_END	1
#define	STAT_BEG	2
#define	STAT_DATA	3


/*
Die Funktion parst einen Markup-Bereich, der durch mindestens
<lim> hintereinanderfolgende Zeichen <key> abgeschlossen wird.
Wird für Verarbeitungsanweisungen, Kommentare und CDATA benötigt.
Das Flag <entity> bestimmt, ob Entitys interpretiert werden.
*/

static void parse_markup (XMLBuf *xml, IO *in, int key, int lim, int entity)
{
	int cnt;
	int32_t c;

	XMLBuf_start(xml);
	cnt = 0;

	while ((c = io_getucs(in)) != EOF)
	{
		if	(cnt >= lim && c == '>')
		{
			xml->sbuf.pos -= cnt;
			break;
		}

#if	0
		/* TODO */
		if	(c == '&' && entity)
		{
			parse_entity(xml, in);
			cnt = 0;
			continue;
		}
#endif
		sb_putucs(c, &xml->sbuf);

		if	(c == key)	cnt++;
		else			cnt = 0;
	}
}

static void *parse_instruction (XMLBuf *xml, IO *in)
{
	char *res;
	int last;

	last = XMLBuf_next(xml);
	parse_markup(xml, in, '?', 1, 1);
	res = XMLBuf_action(xml, xml_pi);
	XMLBuf_prev(xml, last);
	return res;
}

static void *parse_comment (XMLBuf *xml, IO *in)
{
	parse_markup(xml, in, '-', 2, 1);
	return XMLBuf_action(xml, xml_comment);
}

static void *parse_special (XMLBuf *xml, IO *in)
{
	int32_t c = io_getucs(in);

	if	(c == '-')
	{
		c = io_getucs(in);

		if	(c == '-')
			return parse_comment(xml, in);

		sb_putucs(c, &xml->sbuf);
		c = '-';
	}

	sb_putucs(c, &xml->sbuf);
	
	while ((c = io_getucs(in)) != EOF && c != '>')
	{
#if	0
		if	(c == '[')
		{
			char *p = sbuf_nul(&xml->sbuf) + xml->data;

			if	(strcmp(p, "[CDATA") == 0)
			{
				nxml_start(xml);
				return parse_cdata(xml, in);
			}
		}

		if	(c == '&')
		{
			parse_entity(xml, in);
		}
		else	sb_putucs(c, &xml->sbuf);
#else
		sb_putucs(c, &xml->sbuf);
#endif
	}

	return XMLBuf_action(xml, xml_decl);
}

static void skip_tag (XMLBuf *xml, IO *io)
{
	int32_t c;

	do	c = io_getucs(io);
	while	(c != EOF && c != '>');
}

static void trim (StrBuf *sb)
{
	while (sb->pos && SPACE(sb->data[sb->pos - 1]))
		sb->pos--;
}

static void *entry (XMLBuf *xml, int32_t c, IO *io)
{
	void *res;
	int open_tag;
	int pending;
	int last;
	int quote;

	XMLBuf_start(xml);

	while (c != EOF && !END_NAME(c))
	{
		sb_putucs(c, &xml->sbuf);
		c = io_getucs(io);
	}

	while (c == ' ' || c == '\t')
		c = io_getucs(io);

	if	(c == EOF)
		return XMLBuf_err(xml, E_EOF);

	last = XMLBuf_next(xml);
	open_tag = 0;

	if	(c != EOF && c != '>')
	{
		open_tag = 1;

		while (c == ' ' || c == '\t')
			c = io_getucs(io);

		quote = 0;

		while (c != EOF)
		{
			if	(c == '"')
			{
				quote = !quote;
				sb_putucs(c, &xml->sbuf);
			}
			else if	(quote)
			{
				sb_putucs(c, &xml->sbuf);
			}
			else if	(c == '/')
			{
				c = io_getucs(io);

				if	(c == EOF)
				{
					return XMLBuf_err(xml, E_EOF);
				}
				else if	(c != '>')
				{
					return XMLBuf_err(xml, E_CHAR, c);
				}

				res = XMLBuf_action(xml, xml_tag);
				XMLBuf_prev(xml, last);
				return res;
			}
			else if	(c == '>')
			{
				break;
			}

			c = io_getucs(io);
		}

		res = XMLBuf_action(xml, xml_beg);

		if	(res)
			return res;

		XMLBuf_start(xml);
	}

	pending = 0;

	while ((c = io_getucs(io)) != EOF)
	{
		if	(c == '<')
		{
			c = io_getucs(io);

			if	(c == '/')
			{
				while (c != EOF && c != '>')
					c = io_getucs(io);

				break;
			}

			if	(!open_tag)
			{
				res = XMLBuf_action(xml, xml_beg);

				if	(res)
					return res;

				open_tag = 1;
			}

			if	(pending)
			{
				res = XMLBuf_action(xml, xml_data);

				if	(res)
					return res;

				XMLBuf_start(xml);
			}

			if	(c == '?')
			{
				skip_tag(xml, io);
			}
			else if	(c == '!')
			{
				skip_tag(xml, io);
			}
			else if	((res = entry(xml, c, io)))
			{
				return res;
			}
		}
		else if	(pending || !SPACE(c))
		{
			sb_putucs(c, &xml->sbuf);
			pending = 1;
		}
		else if	(c == '\n' && !open_tag)
		{
			res = XMLBuf_action(xml, xml_beg);

			if	(res)
				return res;

			open_tag = 1;
			XMLBuf_start(xml);
		}
	}

	if	(open_tag)
	{
		if	(pending)
		{
			trim(&xml->sbuf);
			res = XMLBuf_action(xml, xml_data);

			if	(res)
				return res;

			XMLBuf_start(xml);
			pending = 0;
		}

		res = XMLBuf_action(xml, xml_end);
	}
	else
	{
		trim(&xml->sbuf);
		res = XMLBuf_action(xml, xml_entry);
	}

	XMLBuf_prev(xml, last);
	return res;
}

void *XMLBuf_parse (XMLBuf *xml, IO *io)
{
	int32_t c;

	if	(!xml || !io)
		return NULL;

	while ((c = io_getucs(io)) != EOF)
	{
		if	(c == '<')
		{
			c = io_getucs(io);

			if	(c == '?')
			{
				parse_instruction(xml, io);
			}
			else if	(c == '!')
			{
				parse_special(xml, io);
			}
			else if	(c == '/')
			{
				break;
			}
			else	return entry(xml, c, io);
		}
		else if	(!SPACE(c))
		{
			io_ungetucs(c, io);
			return XMLBuf_err(xml, E_CHAR, c);
			break;
		}
	}

	return NULL;
}
