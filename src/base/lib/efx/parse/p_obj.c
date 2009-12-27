/*	Parser für Objekte
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>
#include <EFEU/parsedef.h>
#include <ctype.h>


/*	Basisobjekt ohne Operatoren
*/

Obj_t *Parse_obj(io_t *io, int flags)
{
	void *p;
	Obj_t *obj;
	ParseDef_t *parse;
	Type_t *type;

	obj = NULL;
	p = NULL;

	switch (io_scan(io, flags, &p) & SCAN_TYPEMASK)
	{
	case SCAN_INT:

		return NewObj(&Type_int, p);

	case SCAN_LONG:

		return NewObj(&Type_long, p);

	case SCAN_DOUBLE:

		return NewObj(&Type_double, p);

	case SCAN_NULL:

		return ptr2Obj(NULL);

	case SCAN_STR:

		return str2Obj(p);

	case SCAN_CHAR:

		obj = char2Obj(* (char *) p);
		break;

	case SCAN_NAME:		

		if	((type = GetType(p)) != NULL)
		{
			obj = PFunc_type(io, type);
		}
		else if	((parse = GetParseDef(p)) != NULL)
		{
			obj = (*parse->func)(io, parse->data);
		}
		else	return NewPtrObj(&Type_name, p);

		break;

	default:

		break;
	}

	memfree(p);
	return obj;
}
