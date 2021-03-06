#include <EFEU/xml.h>

#define	out_putc(c, out)	out->put(c, out->par)

static void out_puts (const char *data, XMLOutput *out)
{
	while (*data)
		out_putc(*data++, out);
}

static void out_xputs (const char *data, XMLOutput *out)
{
	if	(data)
	{
		out_putc('"', out);

		while (*data)
		{
			switch (*data)
			{
			case '\n':	out_puts("\\n", out); break;
			case '\t':	out_puts("\\t", out); break;
			case '"':	out_puts("\\\"", out); break;
			case '\\':	out_puts("\\\\", out); break;
			default:	out_putc(*data, out); break;
			}

			data++;
		}

		out_putc('"', out);
	}
	else	out_puts("NULL", out);
}

void *xml_dump (XMLBuf *xml, const char *name, const char *data, void *par)
{
	XMLOutput *out = par;
	char buf[20];

	if	(!out || !out->put)	return NULL;

	switch (xml->type)
	{
	case	xml_pi:		out_puts("pi   ", out); break;
	case	xml_dtd:	out_puts("dtd  ", out); break;

	case	xml_beg:	out_puts("beg  ", out); break;
	case	xml_att:	out_puts("att  ", out); break;
	case	xml_data:	out_puts("data ", out); break;
	case	xml_cdata:	out_puts("cdata", out); break;
	case	xml_end:	out_puts("end  ", out); break;

	case	xml_comm:	out_puts("comm ", out); break;
	case	xml_err:	out_puts("err  ", out); break;
	}

	sprintf(buf, " %2d %d", xml->depth, xml->open_tag);
	out_puts(buf, out);

	out_puts(" path=", out);
	out_xputs((char *) xml->sbuf.data, out);
	out_puts(" name=", out);
	out_xputs(name, out);
	out_puts(" data=", out);
	out_xputs(data, out);
	out_putc('\n', out);
	return NULL;
}
