/*	Datenwerte löschen/kopieren/verschieben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>


void CleanData(const Type_t *type, void *tg)
{
	if	(type->fclean)
		type->fclean->eval(type->fclean, NULL, &tg);

	if	(type->clean)	type->clean(type, tg);
	else			memset(tg, 0, type->size);
}

void CopyData(const Type_t *type, void *tg, const void *src)
{
	if	(type->copy)	type->copy(type, tg, src);
	else			memcpy(tg, src, type->size);

	if	(type->fcopy)
	{
		void *arg = (void *) src;
		type->fcopy->eval(type->fcopy, tg, &arg);
	}
}


void AssignData(const Type_t *type, void *tg, const void *src)
{
	if	(tg == src)	return;

	CleanData(type, tg);
	CopyData(type, tg, src);
}


/*	Vektordaten
*/

void CleanVecData(const Type_t *type, size_t dim, void *tg)
{
	while (dim > 0)
	{
		CleanData(type, tg);
		tg = ((char *) tg) + type->size;
		dim--;
	}
}


void CopyVecData(const Type_t *type, size_t dim, void *tg, const void *src)
{
	while (dim > 0)
	{
		CopyData(type, tg, src);
		tg = ((char *) tg) + type->size;
		src = ((char *) src) + type->size;
		dim--;
	}
}


void AssignVecData(const Type_t *type, size_t dim, void *tg, const void *src)
{
	if	(tg != src)
	{
		CleanVecData(type, dim, tg);
		CopyVecData(type, dim, tg, src);
	}
}
