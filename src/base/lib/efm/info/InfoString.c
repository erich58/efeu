/*	Informationsbasis: String
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>

/*	Info-String
*/

static void str_print (io_t *io, const InfoString_t *data)
{
	io_psub(io, data->fmt);
}

static InfoString_t *str_admin (InfoString_t *tg, const InfoString_t *src)
{
	if	(tg)
	{
		memfree(tg->fmt);
		tg->fmt = NULL;

		if	(src == NULL)
		{
			memfree(tg);
			return NULL;
		}

		if	(src == tg)	return tg;
	}
	else
	{
		tg = memalloc(sizeof(InfoString_t));
		tg->type = &InfoType_mstr;

		if	(!src)	return tg;
	}

	tg->fmt = mstrcpy(src->fmt);
	return tg;
}

Info_t *InfoString_create (char *label)
{
	InfoString_t *x = str_admin(NULL, NULL);
	x->fmt = label;
	return (Info_t *) x;
}

InfoType_t InfoType_cstr = { NULL, (InfoPrint_t) str_print };
InfoType_t InfoType_mstr = { (admin_t) str_admin, (InfoPrint_t) str_print };
