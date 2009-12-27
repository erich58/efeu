/*	Konstante Objekte
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <EFEU/cmdsetup.h>

Obj_t *PFunc_bool (io_t *io, void *data)
{
	return bool2Obj(data != NULL);
}

Obj_t *PFunc_int (io_t *io, void *data)
{
	return int2Obj((int) (size_t) data);
}

Obj_t *PFunc_str (io_t *io, void *data)
{
	return str2Obj(mstrcpy(data));
}

Obj_t *PFunc_type (io_t *io, void *data)
{
	return type2Obj(Parse_type(io, data));
}
