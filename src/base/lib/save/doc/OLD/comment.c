/*	Kommentare lesen/zwischenspeichern
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/Document.h>


static void com_start (Merge_t *merge)
{
	if	(merge->buf == NULL)
		merge->buf = new_strbuf(0);
}


static void line_com (Merge_t *merge)
{
	int c;

	com_start(merge);

	do	c = io_getc(merge->input);
	while	(c == ' ' || c == '\t');

	while (c != '\n')
	{
		if	(c == EOF)	return;

		sb_putc(c, merge->buf);
		c = io_getc(merge->input);
	}
	
	sb_putc(c, merge->buf);

	if	(merge->hmode > 1)
		io_ungetc(c, merge->input);
}

static void cstyle_com (Merge_t *merge)
{
	int indent, c, depth;

	com_start(merge);
	sb_clear(merge->buf);
	c = io_getc(merge->input);

	for (depth = 0; c == '\t'; depth++)
		c = io_getc(merge->input);

	while (c == '\n' && depth == 0)
		c = io_getc(merge->input);

	indent = 0;

	while (c != EOF)
	{
		if	(c == '*')
		{
			c = io_getc(merge->input);

			if	(c == '/')
				break;
			
			io_ungetc(c, merge->input);
		}

		if	(c == '\t' && indent)
		{
			indent--;
		}
		else
		{
			indent = (c == '\n') ? depth : 0;
			sb_putc(c, merge->buf);
		}

		c = io_getc(merge->input);
	}
}


int Merge_comment (Merge_t *merge, int c)
{
	if	(c != '/')	return 0;

	switch ((c = io_getc(merge->input)))
	{
	case '*':

		cstyle_com(merge);

		/*
		if	(merge->ignspace)
		{
			c = io_getc(merge->input);

			if	(c != '\n')
				io_ungetc(c, merge->input);
		}
		*/

		return 1;

	case '/':

		line_com(merge);
		return 1;

	default:

		io_ungetc(c, merge->input);
		return 0;
	}
}
