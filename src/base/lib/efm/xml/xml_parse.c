#include <EFEU/xml.h>
#include <ctype.h>

#define	BLANK(c)	((c) == ' ' || c == '\t')
#define	SPACE(c)	(BLANK(c) || c == '\n' || c == '\r')
#define	END_NAME(c)	((c) == '>' || (c) == '/' || SPACE(c))

#define E_EOF   "unexpected end of file"
#define E_CHAR  "unexpected character %#x"
#define E_SEQ	"unexpected character sequence '%s'"

#define	STAT_SKIP	0
#define	STAT_END	1
#define	STAT_BEG	2
#define	STAT_DATA	3

static void *err_char (XMLBuf *xml, unsigned c)
{
	if	(c == EOF)
	{
		return XMLBuf_err(xml, E_EOF);
	}
	else	return XMLBuf_err(xml, E_CHAR, c);
}

static void parse_entity (XMLBuf *xml, IO *in)
{
	int pos;
	int c;
	char *p;
	
	pos = xml->sbuf.pos;
	sb_putc('&', &xml->sbuf);

	while ((c = io_getucs(in)) != EOF && c != ';')
		sb_putucs(c, &xml->sbuf);

	p = sb_nul(&xml->sbuf);

	if	((p = XMLBuf_entity(xml, p + pos + 1)))
	{
		xml->sbuf.pos = pos;
		sb_sync(&xml->sbuf);
		sb_puts(p, &xml->sbuf);
	}
	else

	sb_putc(';', &xml->sbuf);
}

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

	XMLBuf_start(xml, NULL);
	cnt = 0;

	while ((c = io_getucs(in)) != EOF)
	{
		if	(cnt >= lim && c == '>')
		{
			xml->sbuf.pos -= cnt;
			break;
		}

		if	(c == '&' && entity)
		{
			parse_entity(xml, in);
			cnt = 0;
			continue;
		}

		sb_putucs(c, &xml->sbuf);

		if	(c == key)	cnt++;
		else			cnt = 0;
	}
}

static void *parse_instruction (XMLBuf *xml, IO *in)
{
	int last = XMLBuf_next(xml, 0);
	parse_markup(xml, in, '?', 1, 1);
	return XMLBuf_action(xml, xml_pi, last);
}

static void *parse_dtd (XMLBuf *xml, IO *in)
{
	int last;
	int c;

	XMLBuf_start(xml, NULL);

	while ((c = io_getucs(in)) != EOF && c != '>')
	{
		if	(c == '[')
		{

			last = XMLBuf_next(xml, 0);
			parse_markup(xml, in, ']', 1, 0);
			return XMLBuf_action(xml, xml_dtd, last);
		}
		else	sb_putucs(c, &xml->sbuf);
	}

	last = XMLBuf_next(xml, 0);
	return XMLBuf_action(xml, xml_dtd, last);
}

static void *parse_comment (XMLBuf *xml, IO *in)
{
	parse_markup(xml, in, '-', 2, 1);
	return XMLBuf_action(xml, xml_comm, -1);
}

static void *parse_cdata (XMLBuf *xml, IO *in)
{
	parse_markup(xml, in, ']', 2, 0);
	return XMLBuf_action(xml, xml_cdata, -1);
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

	XMLBuf_start(xml, NULL);
	sb_putucs(c, &xml->sbuf);
	
	while ((c = io_getucs(in)) != EOF && c != '>')
	{
		if	(SPACE(c))
		{
			char *p = sb_nul(&xml->sbuf) + xml->data;

			if	(strcmp(p, "DOCTYPE") == 0)
				return parse_dtd(xml, in);
		}
		else if	(c == '[')
		{
			char *p = sb_nul(&xml->sbuf) + xml->data;

			if	(strcmp(p, "[CDATA") == 0)
				return parse_cdata(xml, in);
		}
		else if	(c == '&')
		{
			parse_entity(xml, in);
		}
		else	sb_putucs(c, &xml->sbuf);
	}

	return XMLBuf_action(xml, xml_comm, -1);
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
	int pending;
	int last;
	int quote;
	int open_tag;

/*	Tag bestimmen
*/
	XMLBuf_start(xml, NULL);
	res = NULL;

	while (!END_NAME(c))
	{
		if	(c == EOF)
			return XMLBuf_err(xml, E_EOF);

		sb_putucs(c, &xml->sbuf);
		c = io_getucs(io);
	}

	last = XMLBuf_next(xml, 1);
	pending = 0;

	res = XMLBuf_action(xml, xml_beg, -1);

	if	(res)
		return res;

	/*	Optionen parsen
	*/

	for (;;)
	{
		if	(c == EOF)
		{
			return XMLBuf_err(xml, E_EOF);
		}
		else if	(c == ' ' || c == '\t')
		{
			c = io_getucs(io);
		}
		else if	(c == '>')
		{
			// parse_data() ;
			break;
		}
		else if	(c == '/')
		{
			c = io_getucs(io);

			if	(c != '>')
				return err_char(xml, c);

			return XMLBuf_action(xml, xml_end, last);
		}

		XMLBuf_start(xml, NULL);

		while (c != '=' && !END_NAME(c))
		{
			if	(c == EOF)
				return XMLBuf_err(xml, E_EOF);

			sb_putucs(c, &xml->sbuf);
			c = io_getucs(io);
		}

		if	(c != '=')
			return err_char(xml, c);

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

				return XMLBuf_action(xml, xml_end, last);
			}
			else if	(c == '>')
			{
				break;
			}

			c = io_getucs(io);
		}
	}

	XMLBuf_start(xml, NULL);
	pending = 0;

	while ((c = io_getucs(io)) != EOF)
	{
		if	(c == '<')
		{
			c = io_getucs(io);

			if	(pending)
			{
				res = XMLBuf_action(xml, xml_data, -1);

				if	(res)
					return res;

				XMLBuf_start(xml, NULL);
			}

			if	(c == '/')
			{
				while (c != EOF && c != '>')
					c = io_getucs(io);

				break;
			}

			res = NULL;

			if	(c == '?')
			{
//				res = parse_instruction(xml, in);
				skip_tag(xml, io);
			}
			else if	(c == '!')
			{
//				res = parse_special(xml, in);
				skip_tag(xml, io);
			}
			else	res = entry(xml, c, io);

			if	(res)
				return res;

			XMLBuf_start(xml, NULL);
		}
		else if	(pending || !SPACE(c))
		{
			/*
			if	(c == '&')
			{
				parse_entity(xml, in);
			}
			else */ sb_putucs(c, &xml->sbuf);

			pending = 1;
		}
	}

	if	(pending)
	{
		trim(&xml->sbuf);

		if	((res = XMLBuf_action(xml, xml_data, -1)))
			return res;

		XMLBuf_start(xml, NULL);
	}

	return XMLBuf_action(xml, xml_end, last);
}


/*	Hauptroutine, außerhalb des root-Tags
*/

void *XMLBuf_parse (XMLBuf *xml, IO *io)
{
	int32_t c;
	void *res;

	if	(!xml || !io)
		return NULL;

	while ((c = io_getucs(io)) != EOF)
	{
		if	(c == '<')
		{
			c = io_getucs(io);

			if	(c == '?')
			{
				if	((res = parse_instruction(xml, io)))
					return res;
			}
			else if	(c == '!')
			{
				if	((res = parse_special(xml, io)))
					return res;
			}
			else if	(c == '/')
			{
				return XMLBuf_err(xml, E_SEQ, "</");
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
