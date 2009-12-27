/*	Ein/Ausgabefunktionen
	(c) 1998 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include <EFEU/io.h>
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

size_t io_dbread (io_t *io, void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;

	if	(io == NULL || io->stat != IO_STAT_OK)
	{
		io_error(io, MSG_FTOOLS, 21, 0);
		return 0;
	}

	if	(io->dbread && !io->nsave)
		return io->dbread(io->data, dptr, recl, size, n);

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			if	((c = io_getc(io)) == EOF)
				io_error(io, MSG_FTOOLS, 21, 0);

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

size_t io_dbwrite (io_t *io, void *dptr, size_t recl, size_t size, size_t n)
{
	register size_t i, j;
	register int c;
	
	if	(io && io->dbwrite)
		return io->dbwrite(io->data, dptr, recl, size, n);

	for (i = 0; i < n; i++)
	{
		register unsigned char *data = DATA(dptr, size);

		for (j = 0; j < recl; j++)
		{
			c = GET(data);

			if	(io_putc(c, io) == EOF)
				io_error(io, MSG_FTOOLS, 22, 0);
		}

		dptr = (char *) dptr + size;
	}

	return recl * n;
}
