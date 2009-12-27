/*	Informationskennung zerlegen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/patcmp.h>
#include <ctype.h>

#define	NAME_SPEC(c)	((c) == INFO_SEP || (c) == INFO_ESC)

char *InfoNameToken (char **pptr)
{
	char *ptr;
	strbuf_t *sb;

	if	(pptr == NULL || *pptr == NULL)
		return NULL;

	sb = new_strbuf(0);

	for (ptr = *pptr; *ptr != 0; ptr++)
	{
		if	(*ptr == INFO_ESC && NAME_SPEC(ptr[1]))
		{
			ptr++;
			sb_putc(*ptr, sb);
		}
		else if	(*ptr == INFO_SEP)
		{
			ptr++;
			break;
		}
		else	sb_putc(*ptr, sb);
	}

	*pptr = *ptr ? ptr : NULL;
	return sb2str(sb);
}

int InfoName(io_t *io, InfoNode_t *base, InfoNode_t *info)
{
	if	(info && info != base)
	{
		if	(InfoName(io, base, info->prev))
			io_putc(INFO_SEP, io);

		if	(info->name)
		{
			register char *p = info->name;

			while (*p != 0)
			{
				if	(NAME_SPEC(*p))
					io_putc(INFO_ESC, io);

				io_putc(*p, io);
				p++;
			}
		}

		return 1;
	}
	else	return 0;
}
