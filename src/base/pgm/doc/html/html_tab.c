#include <HTML.h>

char *HTML_colgrp (HTML *html)
{
	char **p = vb_data(&html->colgrp, html->cpos);
	html->cpos++;
	return p ? *p : "<TD>";
}

static void add_col(HTML *html, const char *cdef)
{
	const char **p = vb_next(&html->colgrp);
	*p = cdef;
}

void HTML_coldef (HTML *html, const char *def)
{
	if	(!def)	def = "";

	for (html->colgrp.used = 0; *def != 0; def++)
	{
		switch (*def)
		{
		case 'l':	add_col(html, "<TD align=\"left\">"); break;
		case 'c':	add_col(html, "<TD align=\"center\">"); break;
		case 'r':	add_col(html, "<TD align=\"right\">"); break;
		default:	break;
		}
	}
}
