/*	Beschreibungstext ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>


void PrintInfo (io_t *io, InfoNode_t *base, const char *name)
{
	InfoNode_t *info = GetInfo(base, name);

	if	(info)
	{
		if	(info->load)
			info->load(info);

		if	(info->label)
		{
			io_psub(io, info->label);
			io_putc('\n', io);
		}

		if	(!info->func)
		{
			reg_cpy(1, info->name);
			reg_cpy(2, info->label);
			io_psub(io, info->par);
		}
		else	info->func(io, info);

		if	(info->list)
		{
			int i = info->list->used;
			InfoNode_t **ip = info->list->data;

			io_putc('\n', io);

			while (i > 0)
			{
				InfoName(io, info, *ip);

				if	((*ip)->label)
				{
					io_putc('\t', io);
					io_psub(io, (*ip)->label);
				}

				io_putc('\n', io);
				i--;
				ip++;
			}
		}
	}
}
