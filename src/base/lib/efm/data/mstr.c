/*	Zeichenketten
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/data.h>

static size_t do_io (iofunc_t func, void *ptr, mstr_t *str, int mode)
{
	size_t size, recl;
	ushort_t len;

	size = *str ? strlen(*str) + 1 : 0;
	len = size;

	if	(len < size)
	{
		size = len;
	}

	recl = func(ptr, &len, mode ? sizeof(ushort_t) : 0, sizeof(ushort_t), 1);

	if	(len != size)
	{
		memfree(*str);
		*str = memalloc(len);
	}

	if	(mode)
		recl += func(ptr, *str, 1, 1, len);

	return recl;
}

#if	0
size_t mstr_ioskip (iofunc_t func, void *ptr, mstr_t *str, size_t n)
{
	register size_t recl;

	for (recl = 0; n > 0; n--, str++)
		recl += do_io (func, ptr, str, 0);

	return recl;
}
#endif

size_t mstr_iodata (iofunc_t func, void *ptr, mstr_t *str, size_t n)
{
	register size_t recl;

	for (recl = 0; n > 0; n--, str++)
		recl += do_io (func, ptr, str, 1);

	return recl;
}

void mstr_clrdata (mstr_t *str, size_t n)
{
	for (; n > 0; n--, str++)
	{
		memfree(*str);
		*str = NULL;
	}
}
