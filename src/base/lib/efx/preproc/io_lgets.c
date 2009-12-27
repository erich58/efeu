/*	Zeilen aus IO - Struktur in String laden
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/efutil.h>

#define	BSIZE 1024

char *io_lgets(io_t *io, const char *end)
{
	strbuf_t *sb;
	int c;
	int save;
	int len;

	if	(io == NULL)	return NULL;

	sb = new_strbuf(BSIZE);
	save = 0;
	len = end ? strlen(end) : -1;

	while ((c = io_getc(io)) != EOF)
	{
		if	(c == '\n')
		{
			if	(sb->pos - save == len && strncmp((char *)
					sb->data + save, end, len) == 0)
			{
				sb_setpos(sb, save);
				break;
			}

			save = sb->pos + 1;
			io_linemark(io);
			continue;
		}

		sb_putc(c, sb);
	}

	return sb2str(sb);
}
