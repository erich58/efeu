/*	Informationsbasis: Ausgabefunktiom
	(c) 1998 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6
*/

#include <EFEU/Info.h>
#include <EFEU/mstring.h>
#include <EFEU/parsub.h>
#include <ctype.h>

static void func_print (io_t *io, const InfoFunc_t *data)
{
	data->func(io);
}

InfoType_t InfoType_func = { NULL, (InfoPrint_t) func_print };
