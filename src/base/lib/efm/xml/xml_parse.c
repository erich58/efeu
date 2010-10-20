#include <EFEU/xml.h>
#include <ctype.h>

#define	BLANK(c)	((c) == ' ' || c == '\t')
#define	SPACE(c)	(BLANK(c) || c == '\n' || c == '\r')
#define	END_NAME(c)	((c) == '>' || BLANK(c))

#define	STAT_SKIP	0
#define	STAT_END	1
#define	STAT_BEG	2
#define	STAT_DATA	3

static void skip_tag (XMLBuf *xml, int32_t (*get) (void *par), void *par)
{
	int32_t c;

	do	c = get(par);
	while	(c != EOF && c != '>');
}

static void trim (StrBuf *sb)
{
	while (sb->pos && SPACE(sb->data[sb->pos - 1]))
		sb->pos--;
}

static void *entry (XMLBuf *xml, int32_t c,
	int32_t (*get) (void *par), void *par)
{
	void *res;
	int open_tag;
	int pending;
	int last;

	XMLBuf_start(xml);

	while (c != EOF && !END_NAME(c))
	{
		sb_putucs(c, &xml->sbuf);
		c = get(par);
	}

	last = XMLBuf_next(xml);
	open_tag = 0;

	if	(c != EOF && c != '>')
	{
		open_tag = 1;

		while (c == ' ' || c == '\t')
			c = get(par);

		while (c != EOF && c != '>')
		{
			sb_putucs(c, &xml->sbuf);
			c = get(par);
		}

		res = XMLBuf_action(xml, xml_beg);

		if	(res)
			return res;

		XMLBuf_start(xml);
	}

	pending = 0;

	while ((c = get(par)) != EOF)
	{
		if	(c == '<')
		{
			c = get(par);

			if	(c == '/')
			{
				while (c != EOF && c != '>')
					c = get(par);

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
				skip_tag(xml, get, par);
			}
			else if	(c == '!')
			{
				skip_tag(xml, get, par);
			}
			else if	((res = entry(xml, c, get, par)))
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

void *XMLBuf_parse (XMLBuf *xml, int32_t (*get) (void *par), void *par)
{
	int32_t c;

	if	(!xml || !get)
		return NULL;

	while ((c = get(par)) != EOF)
	{
		if	(c == '<')
		{
			c = get(par);

			if	(c == '?')
			{
				skip_tag(xml, get, par);
			}
			else if	(c == '!')
			{
				skip_tag(xml, get, par);
			}
			else if	(c == '/')
			{
				break;
			}
			else	return entry(xml, c, get, par);
		}
		else if	(!SPACE(c))
		{
			break;
		}
	}

	return NULL;
}
