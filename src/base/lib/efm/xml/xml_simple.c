/*
Kompakte, einfach gebaute XML-Dateien
*/

#include <EFEU/xml.h>

#define	TAG_DELIM	'/'

#define LINE_END(c)	((c) == EOF || (c) == '\n')
#define SPACE(c)	((c) == ' '  || (c) == '\t')
#define TAG_END(c)	(LINE_END(c) || (c) == '=' || SPACE(c))
#define TSEP_IN(c)	((c) == '/')
#define TSEP_BUF(c)	((c) == '/' || (c) == 0)

static void out_data (XMLOutput *out, const char *data)
{
	if	(!data)	return;

	while (*data)
		out->put(*data++, out->par);
}

void *xml_simple_print (XMLBuf *xml, const char *name,
	const char *data, void *par)
{
	XMLOutput *out = par;

	if	(!out || !out->put)	return NULL;

	switch (xml->stat)
	{
	case xml_data:
	case xml_entry:
		out_data(out, (char *) xml->sbuf.data);
		out->put('\t', out->par);
		out_data(out, data);
		out->put('\n', out->par);
		break;
	default:
		break;
	}

	return NULL;
}
