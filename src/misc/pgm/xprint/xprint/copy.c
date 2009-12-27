/*	Filterprogramm
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 3.0
*/

#include <EFEU/pconfig.h>
#include <ctype.h>
#include "printpar.h"

void mode_comment(io_t *in, io_t *out)
{
	int c;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == '/')
		{
			c = io_getc(in);

			if	(c == '*')
			{
				io_ctrl(out, PC_FONT, 10);
				io_putc('/', out);
			}
			else
			{
				io_ungetc(c, in);
				c = '/';
			}
		}
		else if	(c == '*')
		{
			c = io_getc(in);

			if	(c == '/')
			{
				io_putc('*', out);
				io_putc('/', out);
				io_ctrl(out, PC_FONT, 0);
				continue;
			}
			else
			{
				io_ungetc(c, in);
				c = '*';
			}
		}

		io_putc(c, out);
	}
}


void mode_control(io_t *in, io_t *out)
{
	int c;
	char *p;

	while ((c = io_getc(in)) != EOF)
	{
		if	(c == '@')
		{
			p = io_mgets(in, "@\n");
			c = io_getc(in);

			if	(p != NULL)
			{
				if	(*p == 'f')
				{
					io_ctrl(out, PC_FONT, atoi(p + 1));
				}

				memfree(p);
				continue;
			}
			else if	(c == EOF)
			{
				break;
			}
		}

		io_putc(c, out);
	}
}
