/*	Ein/Ausgabefunktionen
	(c) 1999 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/ftools.h>

#if	REVBYTEORDER
#define	DATA(ptr,size)	(unsigned char *) ptr + size - 1
#define	SET(ptr,val)	*(ptr--) = val
#define	GET(ptr)	*(ptr--)
#else
#define	DATA(ptr,size)	(unsigned char *) ptr
#define	SET(ptr,val)	*(ptr++) = val
#define	GET(ptr)	*(ptr++)
#endif

/*	Datensätze lesen
*/

size_t dbread (FILE *file, void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;

	if	(recl == size)
	{
		if	(fread_compat(dptr, size, n, file) != n)
			fileerror(file, MSG_FTOOLS, 21, 0);

		return n;
	}

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			if	((c = getc(file)) == EOF)
				fileerror(file, MSG_FTOOLS, 21, 0);

			SET(data, c);
		}

		for (j = recl; j < size; j++)
			SET(data, 0);

		dptr = (char *) dptr + size;
	}

	return recl * n;
}


/*	Datensätze ausgeben
*/

size_t dbwrite (FILE *file, void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;
	
	if	(recl == size)
	{
		if	(fwrite_compat(dptr, size, n, file) != n)
			fileerror(file, MSG_FTOOLS, 22, 0);

		return n;
	}

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			c = GET(data);

			if	(putc(c, file) == EOF)
				fileerror(file, MSG_FTOOLS, 22, 0);
		}

		dptr = (char *) dptr + size;
	}

	return recl;
}
