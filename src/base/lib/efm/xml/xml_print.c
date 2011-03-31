#include <EFEU/xml.h>

#define	out_char(out, c)	out->put(c, out->par)

typedef struct {
	int beautified;
	int open_tag;
} PCTRL;

static void out_str (XMLOutput *out, const char *name)
{
	if	(!name)	return;

	while (*name)
		out->put(*name++, out->par);
}

static void out_data (XMLOutput *out, const char *name)
{
	if	(!name)	return;

	while (*name)
	{
		switch (*name)
		{
		case '&':	out_str(out, "&amp;"); break;
		case '<':	out_str(out, "&lt;"); break;
		case '>':	out_str(out, "&gt;"); break;
		default:	out_char(out, *name); break;
		}

		++name;
	}
}

static void out_att (XMLOutput *out, const char *name)
{
	if	(!name)	name = "";

	out_char(out, '"');

	while (*name)
	{
		switch (*name)
		{
		case '&':	out_str(out, "&amp;"); break;
		case '<':	out_str(out, "&lt;"); break;
		case '>':	out_str(out, "&gt;"); break;
		case '"':	out_str(out, "&quot;"); break;
		default:	out_char(out, *name); break;
		}

		++name;
	}

	out_char(out, '"');
}

static void out_indent (XMLBuf *xml, XMLOutput *out, int base)
{
	for (base += 2 * xml->depth; base > 0; base--)
		out->put(' ', out->par);
}

static void *do_print (XMLBuf *xml, const char *name, const char *data,
	XMLOutput *out, PCTRL *ctrl)
{
	int need_newline = ctrl->beautified;

	if	(!out || !out->put)	return NULL;

	if	(xml->open_tag)
	{
		switch (xml->type)
		{
		case xml_end:
			out_str(out, "/>");

			if	(ctrl->beautified)
				out_char(out, '\n');

			return NULL;
		case xml_att:
			out_char(out, ' ');
			break;
		default:
			out_char(out, '>');

			if	(ctrl->beautified)
			{
				out_char(out, '\n');
				out_indent(xml, out, 0);
			}

			break;
		}
	}
	else if	(ctrl->beautified)
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
			out_str(out, "[");
			out_str(out, data);
			out_str(out, "]");
		}

		out_char(out, '>');
		need_newline = 1;
		break;
	case xml_beg:
		out_char(out, '<');
		out_str(out, name);
		ctrl->open_tag = 1;
		need_newline = 0;
		break;
	case xml_data:
		out_data(out, data);
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
		if	(ctrl->beautified)
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
		if	(ctrl->beautified)
		{
			out_str(out, "<!--");
			out_str(out, data);
			out_str(out, "-->");
		}
		else	return NULL;

		break;
	case xml_att:
		out_str(out, name);
		out_char(out, '=');
		out_att(out, data);
		need_newline = 0;
		break;
	}

	if	(need_newline)
		out_char(out, '\n');
		
	return NULL;
}

void *xml_print (XMLBuf *xml, const char *name, const char *data, void *par)
{
	PCTRL pctrl = { 1 };
	return do_print(xml, name, data, par, &pctrl);
}

void *xml_compact (XMLBuf *xml, const char *name, const char *data, void *par)
{
	PCTRL pctrl = { 0 };
	return do_print(xml, name, data, par, &pctrl);
}
