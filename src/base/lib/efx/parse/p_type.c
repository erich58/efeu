/*	Datentype bestimmen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

Type_t *Parse_type(io_t *io, Type_t *type)
{
	Obj_t *obj;

	if	(type == NULL)
	{
		obj = Parse_obj(io, SCAN_NAME);

		if	(obj && obj->type == &Type_type)
			type = Val_type(obj->data);

		UnrefObj(obj);
	}

	if	(type == NULL)
		io_error(io, MSG_EFMAIN, 123, 0);

	while (io_eat(io, " \t") == '[')
	{
		io_getc(io);
		obj = EvalObj(Parse_index(io), &Type_int);

		if	(obj == NULL)	return NULL;

		type = NewVecType(type, Val_int(obj->data));
		UnrefObj(obj);
	}

	return type;
}
