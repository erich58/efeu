#include <EFEU/xml.h>

#define	TAG_DELIM	'/'

static XML_DECL(create_buf);

int XMLBuf_next (XMLBuf *xml, int concat)
{
	int last = xml->tag;

	xml->tag = xml->data;

	if	(xml->tag && concat)
		xml->sbuf.data[xml->tag - 1] = TAG_DELIM;

	sb_putc(0, &xml->sbuf);
	xml->data = xml->sbuf.pos;
	return last;
}

void XMLBuf_prev (XMLBuf *xml, int last)
{
	xml->data = xml->tag;
	xml->tag = last;

	if	(xml->data)
		xml->sbuf.data[xml->data - 1] = 0;

	xml->sbuf.pos = xml->data;
}


void XMLBuf_start (XMLBuf *xml, const char *name)
{
	xml->sbuf.pos = xml->data;
	sb_sync(&xml->sbuf);
	sb_puts(name, &xml->sbuf);
}

int XMLBuf_last (XMLBuf *xml)
{
	int last;

	if	(xml->tag)
	{
		for (last = xml->tag - 2; last >= 0; last--)
		{
			if	(xml->sbuf.data[last] == TAG_DELIM)
			{
				last++;
				return last;
			}
		}
	}

	return 0;
}

void *XMLBuf_action (XMLBuf *xml, XMLType which, int prev)
{
	void *res = NULL;

	if	(which == xml_end)
		xml->depth--;

	xml->type = which;

	if	(xml->action)
	{
		char *p = sb_nul(&xml->sbuf);
		res = xml->action(xml, p + xml->tag, p + xml->data, xml->par);
	}

	if	(which == xml_beg)
	{
		xml->depth++;
		xml->open_tag = 1;
	}
	else if	(which != xml_att)
	{
		xml->open_tag = 0;
	}

	if	(prev >= 0)
		XMLBuf_prev(xml, prev);

	return res;
}


static void xml_init (XMLBuf *xml, XMLAction action, void *par)
{
	xml->action = action;
	xml->par = par;
	xml->depth = 0;
	xml->open_tag = 0;

	sb_trunc(&xml->sbuf);
	xml->tag = xml->sbuf.pos;
	sb_putc(0, &xml->sbuf);
	xml->data = xml->sbuf.pos;
	sb_nul(&xml->sbuf);
}

void *XMLBuf_close (XMLBuf *xml)
{
	void *par;

	while (xml->tag)
		XMLBuf_end(xml);

	par = xml->par;
	xml_init(xml, NULL, NULL);
	return par;
}


XMLBuf *xml_create (XMLBuf *xml, XMLAction action, void *par)
{
	if	(!xml)	xml = &create_buf;

	xml_init(xml, action, par);
	return xml;
}


/*
Konstruktion von Elementen
*/

void *XMLBuf_beg (XMLBuf *xml, const char *tag)
{
	XMLBuf_start(xml, tag);
	XMLBuf_next(xml, 1);
	return XMLBuf_action(xml, xml_beg, -1);
}

void *XMLBuf_data (XMLBuf *xml, XMLType type, const char *data)
{
	XMLBuf_start(xml, data);
	return XMLBuf_action(xml, type, -1);
}

void *XMLBuf_end (XMLBuf *xml)
{
	XMLBuf_start(xml, NULL);
	return XMLBuf_action(xml, xml_end, XMLBuf_last(xml));
}

void *XMLBuf_entry (XMLBuf *xml, const char *name, const char *data)
{
	void *res;
	int last;

	XMLBuf_start(xml, name);
	last = XMLBuf_next(xml, 1);

	if	((res = XMLBuf_action(xml, xml_beg, -1)))	return res;

	XMLBuf_start(xml, data);

	if	((res = XMLBuf_action(xml, xml_data, -1)))	return res;

	XMLBuf_start(xml, NULL);
	return XMLBuf_action(xml, xml_end, last);
}


void *XMLBuf_fmt (XMLBuf *xml, XMLType type, const char *fmt, ...)
{
	va_list args;

	XMLBuf_start(xml, NULL);
	va_start(args, fmt);
	sb_vprintf(&xml->sbuf, fmt, args);
	va_end(args);
	return XMLBuf_action(xml, type, -1);
}

static void *entry (XMLBuf *xml, XMLType type, int concat,
	const char *name, const char *data)
{
	int last;

	XMLBuf_start(xml, name);
	last = XMLBuf_next(xml, concat);
	XMLBuf_puts(xml, NULL, data);
	return XMLBuf_action(xml, type, last);
}

void *XMLBuf_att (XMLBuf *xml, const char *name, const char *data)
{
	return entry(xml, xml_att, 0, name, data);
}

void *XMLBuf_pi (XMLBuf *xml, const char *name, const char *data)
{
	return entry(xml, xml_pi, 0, name, data);
}

void *XMLBuf_dtd (XMLBuf *xml, const char *name,
	const char *id, const char *decl)
{
	int last;

	XMLBuf_start(xml, name);
	XMLBuf_puts(xml, " ", id);
	last = XMLBuf_next(xml, 0);
	XMLBuf_puts(xml, NULL, decl);
	return XMLBuf_action(xml, xml_dtd, last);
}

void *XMLBuf_err (XMLBuf *xml, const char *fmt, ...)
{
	va_list args;

	XMLBuf_start(xml, NULL);
	va_start(args, fmt);
	sb_vprintf(&xml->sbuf, fmt, args);
	va_end(args);
	return XMLBuf_action(xml, xml_err, -1);
}
