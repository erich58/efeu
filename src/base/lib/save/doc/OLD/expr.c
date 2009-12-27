/*	Befehl auswerten
	(c) 1999 Erich Frühstück
	A-3423 St.Andrä/Wördern, Südtirolergasse 17-21/5
*/

#include <EFEU/EisDoc.h>
#include <EFEU/InputEnc.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <ctype.h>


void EisDocMerge_expr (EisDocMerge_t *merge, int c)
{
	Obj_t *obj;
	io_t *io;

	io = merge->input;

	if	(c == '{')
	{
		obj = Parse_block(io, '}');
		c = ';';
	}
	else
	{
		io_ungetc(c, io);
		obj = Parse_term(io, 0);
		c = io_getc(io);

		if	(obj == NULL)
		{
			io_putc('\\', merge->output);
			return;
		}

		if	(c != ';')
			io_ungetc(c, io);
	}

	obj = EvalObj(obj, NULL);

	if	(obj && c != ';')
	{
		io = io_tmpbuf(0);
		PrintObj(io, obj);

		if	(isalnum(c) || c == '_')
			io_putc(' ', io);

		EisDocMerge_tmpcpy(merge, io);
	}

	UnrefObj(obj);
}
