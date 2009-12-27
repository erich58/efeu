/*	String einlesen/kopieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/io.h>
#include <EFEU/ioctrl.h>
#include <EFEU/patcmp.h>


int io_mcopy(io_t *in, io_t *out, const char *delim)
{
	int n, c;

	for (n = 0; (c = io_mgetc(in, 0)) != EOF; )
	{
		if	(c == '\\')
		{
			c = io_mgetc(in, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!listcmp(delim, c))
			{
				io_putc('\\', out);
				n++;
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, in);
			break;
		}

		io_putc(c, out);
		n++;
	}

	return n;
}


char *io_mgets(io_t *io, const char *delim)
{
	strbuf_t *sb;
	int c;

	sb = new_strbuf(0);

	while ((c = io_mgetc(io, 0)) != EOF)
	{
		if	(c == '\\')
		{
			c = io_mgetc(io, 0);

			if	(c == EOF)
			{
				c = '\\';
			}
			else if	(!listcmp(delim, c))
			{
				sb_putc('\\', sb);
			}
		}
		else if	(listcmp(delim, c))
		{
			io_ungetc(c, io);
			break;
		}

		sb_putc(c, sb);
	}

	return sb2str(sb);
}
