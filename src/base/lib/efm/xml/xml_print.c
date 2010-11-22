#include <EFEU/xml.h>

#define	out_char(out, c)	out->put(c, out->par)

static void out_str (XMLOutput *out, const char *name)
{
	if	(!name)	return;

	while (*name)
		out->put(*name++, out->par);
}

static void out_indent (XMLBuf *xml, XMLOutput *out, int base)
{
	for (base += 2 * xml->depth; base > 0; base--)
		out->put(' ', out->par);
}

static void *do_print (XMLBuf *xml, const char *name, const char *data,
	XMLOutput *out, int beautified)
{
	int need_newline = beautified;

	if	(!out || !out->put)	return NULL;

	if	(xml->open_tag)
	{
		switch (xml->type)
		{
		case xml_end:
			out_str(out, "/>");

			if	(beautified)
				out_char(out, '\n');

			return NULL;
		case xml_att:
			out_char(out, ' ');
			break;
		default:
			out_char(out, '>');

			if	(beautified)
			{
				out_char(out, '\n');
				out_indent(xml, out, 0);
			}

			break;
		}
	}
	else if	(beautified)
	{
		out_indent(xml, out, 0);
	}

	switch (xml->type)
	{
	case xml_pi:
		out_str(out, "<?");
		out_str(out, name);

		if	(data && *data)
		{
			out_char(out, ' ');
			out_str(out, data);
		}

		out_str(out, "?>");
		need_newline = 1;
		break;
	case xml_dtd:
		out_str(out, "<!DOCTYPE ");
		out_str(out, name);

		if	(data && *data)
		{
			out_str(out, "\n[\n");
			out_str(out, data);
			out_str(out, "\n]");
		}

		out_char(out, '>');
		need_newline = 1;
		break;
	case xml_beg:
		out_char(out, '<');
		out_str(out, name);
		need_newline = 0;
		break;
	case xml_data:
		out_str(out, data);
		break;
	case xml_cdata:
		out_str(out, "<![CDATA[");
		out_str(out, data);
		out_str(out, "]]>");
		break;
	case xml_end:
		if	(xml->open_tag)	return NULL;

		out_str(out, "</");
		out_str(out, name);
		out_char(out, '>');
		break;
	case xml_err:
		if	(beautified)
		{
			out_str(out, "<!--\nERROR: ");
			out_str(out, data);
			out_str(out, "\n");
			out_indent(xml, out, 0);
			out_str(out, "-->");
		}
		else	return NULL;

		break;
	case xml_comm:
		if	(beautified)
		{
			out_str(out, "<!--");
			out_str(out, data);
			out_str(out, "-->");
		}
		else	return NULL;

		break;
	case xml_att:
		out_str(out, name);
		out_str(out, "=\"");
		out_str(out, data);
		out_str(out, "\"");
		need_newline = 0;
		break;
	}

	if	(need_newline)
		out_char(out, '\n');
		
	return NULL;
}

void *xml_print (XMLBuf *xml, const char *name, const char *data, void *par)
{
	return do_print(xml, name, data, par, 1);
}

void *xml_compact (XMLBuf *xml, const char *name, const char *data, void *par)
{
	return do_print(xml, name, data, par, 0);
}
