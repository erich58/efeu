/*	Information ausgeben
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/strbuf.h>
#include <EFEU/mstring.h>
#include <ctype.h>

Info_t *Info_admin (Info_t *tg, const Info_t *src)
{
	if	(tg && tg->type && tg->type->admin)
		return tg->type->admin(tg, src);

	if	(src && src->type && src->type->admin)
		return src->type->admin(NULL, src);

	return (Info_t *) src;
}

void Info_print (io_t *io, const Info_t *data)
{
	if	(io && data && data->type && data->type->pfunc)
		data->type->pfunc(io, data);
}

void Info_subprint (io_t *io, const Info_t *data, const char *pfx)
{
	if	(io && data && data->type && data->type->pfunc)
	{
		io = io_lmark(io_refer(io), pfx, NULL, 0);
		data->type->pfunc(io, data);
		io_close(io);
	}
}
