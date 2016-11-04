/*	Datentype einer Matrix bestimmen
	(c) 1994 Erich Frühstück
*/

#include <EFEU/mdmat.h>
#include <EFEU/preproc.h>

EfiType *mdtype (const char *str)
{
	IO *io;
	EfiType *type;
	EfiObj *obj;

	if	(str[0] == '[')
	{
		char *head;
		char *name;
		IO *cin;

		head = mstrcut(str + 1, &name, "]", 0);
		cin = io_cmdpreproc(io_mstr(head));
		CmdEval(cin, NULL);
		io_close(cin);
		str = name;
	}

	if	((type = GetType(str)) != NULL)
		return type;

	if	((type = edb_type(str)) != NULL)
		return type;

/*	Wegen Kompatibilität
*/
	if	(strncmp(str, "struct", 6) != 0)
	{
		EfiStruct *list;

		io = io_cmdpreproc(io_cstr(str));
		list = GetStruct(NULL, io, EOF);
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


char *type2str(const EfiType *type)
{
	StrBuf *sb;
	IO *io;

	sb = sb_acquire();
	io = io_strbuf(sb);
	ShowType(io, type);
	io_close(io);
	return sb_cpyrelease(sb);
}


EfiConv *Md_ConvDef(EfiConv *buf, const EfiType *type1, const EfiType *type2)
{
	EfiConv *konv = GetConv(buf, type1, type2);

	if	(konv == NULL)
		log_note(NULL, "[mdmat:102]",
			"mm", type2str(type1), type2str(type2));

	return konv;
}
