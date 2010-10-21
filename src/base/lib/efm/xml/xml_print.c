#include <EFEU/xml.h>

static void out_tag (XMLOutput *out, const char *start,
	const char *name, const char *opt, const char *end)
{
	while (*start)
		out->put(*start++, out->par);

	while (*name)
		out->put(*name++, out->par);

	if	(opt && *opt)
	{
		out->put(' ', out->par);

		while (*opt)
			out->put(*opt++, out->par);
	}

	while (*end)
		out->put(*end++, out->par);
}

static void out_data (XMLOutput *out, const char *name)
{
	if	(name)
	{
		while (*name)
			out->put(*name++, out->par);
	}
}

#define	out_beg(out, name, opt)	out_tag(out, "<", name, opt, ">")
#define	out_end(out, name)	out_tag(out, "</", name, NULL, ">")

#if	0
static void out_beg (XMLOutput *out, const char *name, const char *opt)
{
	out->put('<', out->par);

	while (*name)
		out->put(*name++, out->par);

	if	(opt && *opt)
	{
		out->put(' ', out->par);

		while (*opt)
			out->put(*opt++, out->par);
	}

	out->put('>', out->par);
}

static void out_end (XMLOutput *out, const char *name)
{
	out->put('<', out->par);
	out->put('/', out->par);

	while (*name)
		out->put(*name++, out->par);

	out->put('>', out->par);
}

#endif

void *xml_print (XMLBuf *xml, const char *name, const char *data, void *par)
{
	XMLOutput *out = par;
	int i;

	if	(!out || !out->put)	return NULL;

	for (i = 2 * xml->depth; i > 0; i--)
		out->put(' ', out->par);

	switch (xml->stat)
	{
	case xml_tag:
		out_tag(out, "<", name, data, "/>");
		break;
	case xml_beg:
		out_tag(out, "<", name, data, ">");
		break;
	case xml_data:
		out_data(out, data);
		break;
	case xml_end:
		out_tag(out, "</", name, NULL, ">");
		break;
	case xml_entry:
		out_tag(out, "<", name, NULL, ">");
		out_data(out, data);
		out_tag(out, "</", name, NULL, ">");
		break;
	case xml_err:
		out_data(out, "<!--\nERROR: ");
		out_data(out, data);
		out_data(out, "\n-->");
		break;
	default:
		break;
	}

	out->put('\n', out->par);
	return NULL;
}

void *xml_compact (XMLBuf *xml, const char *name, const char *data, void *par)
{
	XMLOutput *out = par;

	if	(!out || !out->put)	return NULL;

	switch (xml->stat)
	{
	case xml_tag:
		out_tag(out, "<", name, data, "/>");
		break;
	case xml_beg:
		out_beg(out, name, data);
		break;
	case xml_data:
		out_data(out, data);
		break;
	case xml_end:
		out_end(out, name);
		break;
	case xml_entry:
		out_beg(out, name, NULL);
		out_data(out, data);
		out_end(out, name);
		break;
	case xml_pi:
	default:
		break;
	}

	return NULL;
}
