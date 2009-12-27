/*	Eingabe - Preprozessor
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/efmain.h>
#include <EFEU/efio.h>
#include <EFEU/object.h>
#include <EFEU/preproc.h>
#include <EFEU/MakeDepend.h>
#include <ctype.h>

#define	BREAK_COL	70	/* Spaltenpostion für Fortsetzungszeile */

void MakeDependList (io_t *io, const char *name)
{
	int n, p;
	char **ptr;

	if	(name == NULL)	return;

	n = DependList.used;
	ptr = DependList.data;

	if	(n == 0)	return;

	io_putc('\n', io);
	p = io_puts(name, io);
	p += io_puts(":", io);

	while (n > 0)
	{
		io_putc(' ', io);
		p++;

		if	(*ptr && p + strlen(*ptr) >= BREAK_COL)
		{
			io_puts("\\\n ", io);
			p = 1;
		}
			
		p += io_puts(*ptr, io);
		ptr++;
		n--;
	}

	io_putc('\n', io);
}

void Func_target (Func_t *func, void *rval, void **arg)
{
	AddTarget(Val_str(arg[0]));
}

void Func_depend (Func_t *func, void *rval, void **arg)
{
	AddDepend(Val_str(arg[0]));
}

void Func_makedepend (Func_t *func, void *rval, void **arg)
{
	MakeDependList(Val_io(arg[1]), Val_str(arg[0]));
}

static ObjList_t *make_list (vecbuf_t *buf)
{
	ObjList_t *list, **ptr;
	int n;
	char **dep;

	list = NULL;
	ptr = &list;

	for (n = buf->used, dep = buf->data; n-- > 0; dep++)
	{
		*ptr = NewObjList(str2Obj(mstrcpy(*dep)));
		ptr = &(*ptr)->next;
	}

	return list;
}

void Func_dependlist (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = make_list(&DependList);
}

void Func_targetlist (Func_t *func, void *rval, void **arg)
{
	Val_list(rval) = make_list(&TargetList);
}
