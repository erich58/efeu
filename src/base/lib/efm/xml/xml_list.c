#include <EFEU/xml.h>

static void out_data (XMLOutput *out, const char *data)
{
	if	(!data)	return;

	while (*data)
		out->put(*data++, out->par);
}

void *xml_list (XMLBuf *xml, const char *name, const char *data, void *par)
{
	XMLOutput *out = par;

	if	(!out || !out->put)	return NULL;

	switch (xml->stat)
	{
	case xml_data:
	case xml_entry:
		out_data(out, (char *) xml->sbuf.data);
		out->put('=', out->par);
		out_data(out, data);
		out->put('\n', out->par);
		break;
	case xml_beg:
		if	(data && *data)
		{
			out_data(out, (char *) xml->sbuf.data);
			out->put(' ', out->par);
			out_data(out, data);
			out->put('\n', out->par);
		}

		break;
	default:
		break;
	}

	return NULL;
}
