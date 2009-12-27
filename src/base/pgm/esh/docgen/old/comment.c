/*	Kommentare lesen/zwischenspeichern
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"
#include <ctype.h>


static void com_start(void)
{
	if	(MergeStat.buf == NULL)
		MergeStat.buf = new_strbuf(0);
}

static void line_com(void)
{
	int c;

	com_start();

	do	c = io_getc(MergeStat.in);
	while	(c == ' ' || c == '\t');

	while (c != EOF && c != '\n')
	{
		sb_putc(c, MergeStat.buf);
		c = io_getc(MergeStat.in);
	}

	if	(!MergeStat.linestart && c != EOF)
		io_ungetc(c, MergeStat.in);
}

static void cstyle_com(void)
{
	int indent, c, depth;

	com_start();
	c = io_getc(MergeStat.in);

	for (depth = 0; c == '\t'; depth++)
		c = io_getc(MergeStat.in);

	while (c == '\n' && depth == 0)
		c = io_getc(MergeStat.in);

	indent = 0;

	while (c != EOF)
	{
		if	(c == '*')
		{
			c = io_getc(MergeStat.in);

			if	(c == '/')
				break;
			
			io_ungetc(c, MergeStat.in);
		}

		if	(c == '\t' && indent)
		{
			indent--;
		}
		else
		{
			indent = (c == '\n') ? depth : 0;
			sb_putc(c, MergeStat.buf);
		}

		c = io_getc(MergeStat.in);
	}
}


int SkipComment (int c)
{
	if	(c == MergeStat.comkey)
	{
		line_com();
		return 1;
	}

/*	C++ Style Kommentare
*/
	if	(c != '/' || MergeStat.comkey != 0)
		return 0;

	switch ((c = io_getc(MergeStat.in)))
	{
	case '*':

		cstyle_com();

		if	(MergeStat.linestart)
		{
			c = io_getc(MergeStat.in);

			if	(c != '\n')
			{
				io_ungetc(c, MergeStat.in);
				MergeStat.linestart = 0;
			}
		}

		return 1;

	case '/':

		line_com();
		return 1;

	default:

		io_ungetc(c, MergeStat.in);
		return 0;
	}
}
