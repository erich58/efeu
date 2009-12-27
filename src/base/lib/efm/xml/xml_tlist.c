#include <EFEU/xml.h>

static void out_data (XMLOutput *out, const char *data)
{
	if	(!data)	return;

	while (*data)
		out->put(*data++, out->par);
}

void *xml_tlist (XMLBuf *xml, const char *name, const char *data, void *par)
{
	XMLOutput *out = par;

	if	(!out || !out->put)	return NULL;

	switch (xml->stat)
	{
	case xml_entry:
		out_data(out, name);
		/* FALLTROUGH */
	case xml_data:
		out->put('\t', out->par);
		out_data(out, data);
		out->put('\n', out->par);
		break;
	case xml_beg:
		out->put('+', out->par);
		out_data(out, name);

		if	(data && *data)
		{
			out->put(' ', out->par);
			out_data(out, data);
		}

		out->put('\n', out->par);
		break;
	case xml_end:
		out->put('/', out->par);
		out_data(out, name);
		out->put('\n', out->par);
		break;
	default:
		break;
	}

	return NULL;
}