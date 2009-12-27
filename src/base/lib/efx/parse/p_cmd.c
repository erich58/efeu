/*	Parser für Befehlszeilen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/object.h>

#define	PROMPT	"{ >>> "

/*	Befehlszeile
*/

Obj_t *Parse_cmd(io_t *io)
{
	Obj_t *obj;

	if	(io_eat(io, "%s") == '{')
	{
		io_getc(io);
		return Parse_block(io, '}');
	}

	obj = Parse_term(io, 0);

	if	(io_eat(io, " \t") == ';')
	{
		io_getc(io);

		if	(obj == NULL)	obj = Obj_noop();
	}

	return obj;
}



/*	Blöcke
*/

Obj_t *Parse_block(io_t *io, int endchar)
{
	ObjList_t *list;
	ObjList_t **ptr;
	Obj_t *obj;
	Block_t block;
	char *prompt;
	int c;

	prompt = io_prompt(io, PROMPT);
	PushVarTab(NULL, NULL);
	list = NULL;
	ptr = &list;

	for (;;)
	{
		c = io_eat(io, "%s;");

		if	(c == endchar)
		{
			io_getc(io);
			break;
		}
		else if	(c == EOF)
		{
			io_error(io, MSG_EFMAIN, 110, 0);
			break;
		}
		else if	(c == '{')
		{
			io_getc(io);
			obj = Parse_block(io, '}');
			c = ';';
		}
		else if	((obj = Parse_cmd(io)) == NULL)
		{
			obj = Parse_term(io, 0);

			if	(io_eat(io, " \t") == ';')
			{
				io_getc(io);

				if	(obj == NULL)	obj = Obj_noop();
			}
		}

		if	(obj == NULL)
		{
			io_error(io, MSG_EFMAIN, 117, 0);
			break;
		}
		else if	(obj->type->eval == NULL)
		{
			UnrefObj(obj);
			reg_set(0, io_ident(io));
			errmsg(MSG_EFMAIN, 113);
		}
		else
		{
			*ptr = NewObjList(obj);
			ptr = &(*ptr)->next;
		}
	}

	io_prompt(io, prompt);
	block.tab = RefVarTab(LocalVar);
	block.list = list;
	PopVarTab();
	return NewObj(&Type_block, &block);
}
