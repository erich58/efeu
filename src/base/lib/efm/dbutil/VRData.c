/*	Datenbanken mit variabler Satzlänge
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/VRData.h>

#define	VRBLKSIZE	1024

size_t VRData_read (io_t *io, VRData_t *data, size_t n)
{
	size_t x = 0;

	while (n-- > 0)
	{
		data->recl = io_getval(io, 2);

		if	(data->recl < 4)
		{
			io_error(io, MSG_VRDATA, 3, 0);
			break;
		}
			
		if	(io_getval(io, 2) != 0)
		{
			io_error(io, MSG_VRDATA, 4, 0);
			break; 
		}

		data->recl -= 4;

		if	(data->size < data->recl)
		{
			data->size = sizealign(data->recl, VRBLKSIZE);
			memfree(data->buf);
			data->buf = lmalloc(data->size);
		}

		x += io_read(io, data->buf, data->recl);
		data++;
	}

	return x;
}

size_t VRData_write (io_t *io, VRData_t *data, size_t n)
{
	size_t x = 0;

	while (n-- > 0)
	{
		io_putval(data->recl, io, 2);
		io_putval(0, io, 2);
		x += io_write(io, data->buf, data->recl);
		data++;
	}

	return x;
}

void VRData_clear (VRData_t *data, size_t n)
{
	while (n-- > 0)
	{
		lfree(data->buf);
		data->buf = NULL;
		data->recl = data->size = 0;
		data++;
	}
}
