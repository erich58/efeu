/*	Bereich einlesen
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <efeudoc.h>
#include <ctype.h>

char *DocParseRegion (io_t *in, const char *delim)
{
	strbuf_t *buf;
	int last;
	int c;

	buf = new_strbuf(0);
	last = 0;

	while ((c = io_getc(in)) != EOF)
	{
		if	(delim && c == '\n')
		{
			sb_putc(0, buf);

			if	(patcmp(delim, (char *) buf->data + last, NULL))
			{
				buf->pos = last;
				sb_sync(buf);
				break;
			}

			buf->data[buf->pos - 1] = '\n';
			last = buf->pos;
		}
		else	sb_putc(c, buf);
	}

	return sb2str(buf);
}
