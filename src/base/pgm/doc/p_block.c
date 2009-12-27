/*	Bereich einlesen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <efeudoc.h>
#include <ctype.h>

static void cp_verb (io_t *in, strbuf_t *buf)
{
	int c;
	
	sb_put('|', buf);

	while ((c = io_getc(in)) != EOF)
	{
		sb_put(c, buf);

		if	(c == '|')
		{
			c = io_getc(in);

			if	(c != '|')
			{
				io_ungetc(c, in);
				break;
			}
			else	sb_put(c, buf);
		}
	}
}

static int cp_minus (io_t *in, strbuf_t *buf)
{
	int n, c;

	sb_put('-', buf);

	for (n = 1; (c = io_getc(in)) == '-'; n++)
		sb_put('-', buf);

	io_ungetc(c, in);
	return n;
}

static void cp_line (io_t *in, strbuf_t *buf)
{
	int c;

	while ((c = io_getc(in)) != EOF)
	{
		sb_put(c, buf);

		if	(c == '\n')	break;
	}
}

static void cp_reg (io_t *in, strbuf_t *buf)
{
	int c;

	do
	{
		cp_line(in, buf);
		c = io_getc(in);

		if	(c == '-' && cp_minus(in, buf) > 3)
		{
			cp_line(in, buf);
			return;
		}
		else	sb_put(c, buf);
	}
	while	(c != EOF);
}

char *DocParseBlock (io_t *in, int mode, const char *beg,
	const char *end, const char *toggle)
{
	int nl, c, depth;
	strbuf_t *buf;
	char *p;

	buf = (mode || toggle) ? new_strbuf(0) : NULL;
	nl = 1;
	depth = 0;

	while ((c = io_skipcom(in, NULL, nl)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_getc(in);

			if	(isalpha(c) || c == '_')
			{
				io_ungetc(c, in);
				p = DocParseName(in, 0);

				if	(mstrcmp(p, end) == 0)
				{
					if	(depth == 0)
					{
						DocSkipSpace(in, 1);
						break;
					}
					else	depth--;
				}
				else if	(mstrcmp(p, beg) == 0)
				{
					depth++;
				}
				else if	(depth == 0 && mstrcmp(p, toggle) == 0)
				{
					DocSkipSpace(in, 1);
					toggle = NULL;
					mode = !mode;
					continue;
				}

				if	(mode)
				{
					sb_putc('\\', buf);
					sb_puts(p, buf);
					sb_putc(';', buf);
				}
			}
			else if	(mode)	
			{
				sb_putc('\\', buf);
				sb_putc(c, buf);
			}
		}
		else if	(c == '|')
		{
			cp_verb(in, mode ? buf : NULL);
			nl = 0;
		}
		else if	(nl && c == '-')
		{
			if	(cp_minus(in, mode ? buf : NULL) > 3)
			{
				cp_reg(in, mode ? buf : NULL);
				nl = 1;
			}
			else	nl = 0;
		}
		else
		{
			if	(mode)	sb_putc(c, buf);

			nl = (c == '\n');
		}
	}

	return sb2str(buf);
}
