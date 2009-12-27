/*	Datentype einer Matrix bestimmen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2.0
*/

#include <EFEU/mdmat.h>

Type_t *mdtype(const char *str)
{
	io_t *io;
	Type_t *type;
	Obj_t *obj;

	if	((type = GetType(str)) != NULL)
		return type;

/*	Wegen Kompatibilität
*/
	if	(strncmp(str, "struct", 6) != 0)
	{
		Var_t *list;

		io = io_cstr(str);
		list = GetStruct(io, EOF);
		io_close(io);
		return MakeStruct(NULL, NULL, list);
	}

	io = io_cstr(str);
	obj = EvalObj(Parse_cmd(io), &Type_type);
	io_close(io);
	type = obj ? Val_type(obj->data) : NULL;
	UnrefObj(obj);
	return type;
}


char *type2str(const Type_t *type)
{
	strbuf_t *sb;
	io_t *io;

	sb = new_strbuf(0);
	io = io_strbuf(sb);
	ShowType(io, type);
	io_close(io);
	return sb2str(sb);
}


Konv_t *Md_KonvDef(Konv_t *buf, const Type_t *type1, const Type_t *type2)
{
	Konv_t *konv = GetKonv(buf, type1, type2);

	if	(konv == NULL)
	{
		reg_set(1, type2str(type1));
		reg_set(2, type2str(type2));
		errmsg(MSG_MDMAT, 102);
	}

	return konv;
}
