#include <EFEU/xml.h>

#define	TAG_DELIM	'/'

static XML_DECL(create_buf);

int XMLBuf_next (XMLBuf *xml)
{
	int last = xml->tag;

	xml->tag = xml->data;

	if	(xml->tag)
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

#if	0
	printf("tag=%d, data=%d, path=%s, name=%s\n", xml->tag, xml->data,
		xml->sbuf.data, xml->sbuf.data + xml->tag);
#endif
	xml->sbuf.pos = xml->data;
}


void XMLBuf_start (XMLBuf *xml)
{
	xml->sbuf.pos = xml->data;
	sb_sync(&xml->sbuf);
}

int XMLBuf_last (XMLBuf *xml)
{
	int last;

	if	(xml->tag)
	{
		for (last = xml->tag - 2; last > 0; last--)
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

void *XMLBuf_action (XMLBuf *xml, XMLType which)
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
		xml->depth++;

	return res;
}

static void xml_init (XMLBuf *xml, XMLAction action, void *par)
{
	xml->action = action;
	xml->par = par;
	xml->depth = 0;

	sb_trunc(&xml->sbuf);
	xml->tag = xml->sbuf.pos;
	sb_putc(0, &xml->sbuf);
	xml->data = xml->sbuf.pos;
	sb_nul(&xml->sbuf);
}

void *XMLBuf_close (XMLBuf *xml)
{
	void *par;

	while (xml->tag > 1)
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


void *XMLBuf_beg (XMLBuf *xml, const char *tag, const char *opt)
{
	XMLBuf_start(xml);
	sb_puts(tag, &xml->sbuf);
	XMLBuf_next(xml);
	sb_puts(opt, &xml->sbuf);
	return XMLBuf_action(xml, xml_beg);
}

void *XMLBuf_data (XMLBuf *xml, XMLType type, const char *data)
{
	XMLBuf_start(xml);
	sb_puts(data, &xml->sbuf);
	return XMLBuf_action(xml, type);
}

void *XMLBuf_end (XMLBuf *xml)
{
	void *res;

	XMLBuf_start(xml);
	res = XMLBuf_action(xml, xml_end);
	XMLBuf_prev(xml, XMLBuf_last(xml));
	return res;
}

void *XMLBuf_att (XMLBuf *xml, const char *name, const char *data)
{
	void *res;
	int last;

	XMLBuf_start(xml);
	sb_puts(name, &xml->sbuf);
	last = XMLBuf_next(xml);
	sb_puts(data, &xml->sbuf);
	res = XMLBuf_action(xml, xml_att);
	XMLBuf_prev(xml, last);
	return res;
}

void *XMLBuf_element (XMLBuf *xml, XMLType which,
	const char *name, const char *data)
{
	int last;
	void *res;

	switch (which)
	{
	case xml_pi:
	case xml_att:
	case xml_decl:
		if	(!name)
			return XMLBuf_err(xml, "Name fehlt");
		break;

	case xml_comm:
	case xml_err:
	case xml_data:
	case xml_cdata:
		if	(name)
			return XMLBuf_err(xml, "Name nicht zulässig");

		break;
	case xml_DTDbeg:
	case xml_DTDend:
	case xml_beg:
	case xml_end:
		return XMLBuf_err(xml, "Elementtyp nicht zulässig");
	}

	XMLBuf_start(xml);

	if	(name)
	{
		sb_puts(name, &xml->sbuf);
		last = XMLBuf_next(xml);
	}

	sb_puts(data, &xml->sbuf);
	res = XMLBuf_action(xml, which);

	if	(name)
		XMLBuf_prev(xml, last);

	return res;
}

void *XMLBuf_err (XMLBuf *xml, const char *fmt, ...)
{
	va_list args;

	XMLBuf_start(xml);
	va_start(args, fmt);
	sb_vprintf(&xml->sbuf, fmt, args);
	va_end(args);
	return XMLBuf_action(xml, xml_err);
}
