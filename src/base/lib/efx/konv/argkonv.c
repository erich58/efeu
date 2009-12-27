/*	Funktionsargumente konvertieren
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/konvobj.h>

static Func_t *KonvFunc(const Type_t *old, const Type_t *new)
{
	Func_t **ftab;
	int i;

	if	(old == NULL)	return NULL;

	ftab = old->konv.data;

	for (i = 0; i < old->konv.used; i++)
		if (ftab[i]->type == new) return ftab[i];

	return NULL;
}

ArgKonv_t *GetArgKonv(const Type_t *old, const Type_t *new)
{
	static ArgKonv_t buf;

	buf.type = NULL;
	buf.dist = 0;

	for (; new != NULL; new = new->dim ? NULL : new->base)
	{
		buf.func = NULL;

		if	(old == new)	return &buf;

		if	((buf.func = KonvFunc(old, new)))
		{
			switch (buf.func->weight)
			{
			case KONV_PROMOTION:
				buf.dist |= D_PROMOTE;
				break;
			case KONV_RESTRICTED:
				buf.dist |= D_RESTRICTED;
				break;
			default:
				buf.dist |= D_KONVERT;
				break;
			}

			return &buf;
		}

		buf.type = (Type_t *) new;
		buf.dist = D_EXPAND;
	}

	return NULL;
}

void ArgKonv(ArgKonv_t *konv, void *tg, void *src)
{
	if	(konv->func)
	{
		konv->func->eval(konv->func, tg, &src);
	}
	else if	(konv->type)
	{
		CopyData(konv->type, tg, src);
	}
}

int ArgKonvDist (const Type_t *old, const Type_t *new)
{
	ArgKonv_t *konv = GetArgKonv(old, new);
	return konv ? konv->dist : D_RESTRICTED;
}
