/*	Datenwerte ein/ausgeben
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/stdtype.h>

size_t IOData (const Type_t *t, iofunc_t f, void *p, void *d)
{
	if	(t->iodata)
	{
		return t->iodata(t, f, p, d, 1);
	}
	else	return f(p, d, t->recl, t->size, 1);
}

size_t IOVecData (const Type_t *t, iofunc_t f, void *p, size_t n, void *d)
{
	if	(t->iodata)
	{
		return t->iodata(t, f, p, d, n);
	}
	else	return f(p, d, t->recl, t->size, n);
}

size_t IOData_std (const Type_t *t, iofunc_t f, void *p, void *d, size_t n)
{
	return f(p, d, t->recl, t->size, n);
}

size_t IOData_str (const Type_t *t, iofunc_t f, void *p, void *d, size_t n)
{
	return mstr_iodata(f, p, d, n);
}
