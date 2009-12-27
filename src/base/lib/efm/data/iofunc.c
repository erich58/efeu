/*	Ein/Ausgabefunktionen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/data.h>

#if	REVBYTEORDER
#define	DATA(ptr,size)	(char *) ptr + size - 1
#define	SET(ptr,val)	*(ptr--) = val
#define	GET(ptr)	*(ptr--)
#else
#define	DATA(ptr,size)	(char *) ptr
#define	SET(ptr,val)	*(ptr++) = val
#define	GET(ptr)	*(ptr++)
#endif

/*	Eingabefunktionen
*/

size_t xloaddata (void *ptr, void *dptr, size_t recl, size_t size, size_t n)
{
	register char *data = dptr;
	register size_t i;

	for (i = size * n; i > 0; i--, data++)
		*data = 0;

	return recl * n;
}

size_t mloaddata (void *mptr, void *dptr, size_t recl, size_t size, size_t n)
{
	register char **mem = mptr;
	register size_t i, j;
	
	for (i = 0; i < n; i++)
	{
		register char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
			SET(data, *((*mem)++));

		for (j = recl; j < size; j++)
			SET(data, 0);
		
		dptr = (char *) dptr + size;
	}

	return recl * n;
}

size_t floaddata (void *fptr, void *dptr, size_t recl, size_t size, size_t n)
{
	register FILE *file = fptr;
	register size_t i, j;
	register int c;

	for (i = 0; i < n; i++)
	{
		register char *data = DATA(dptr, size);

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

/*	Ausgabefunktionen
*/

size_t xsavedata (void *ptr, void *data, size_t recl, size_t size, size_t n)
{
	return recl * n;
}

size_t msavedata (void *mptr, void *dptr, size_t recl, size_t size, size_t n)
{
	register char **mem = mptr;
	register size_t i, j;
	
	for (i = 0; i < n; i++)
	{
		register char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
			*((*mem)++) = GET(data);

		dptr = (char *) dptr + size;
	}

	return recl * n;
}

size_t fsavedata (void *fptr, void *dptr, size_t recl, size_t size, size_t n)
{
	register FILE *file = fptr;
	register size_t i, j;
	register int c;
	
	for (i = 0; i < n; i++)
	{
		register char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			c = GET(data);

			if	(putc(c, file) == EOF)
				fileerror(file, MSG_FTOOLS, 22, 0);
		}

		dptr = (char *) dptr + size;
	}

	return recl * n;
}
