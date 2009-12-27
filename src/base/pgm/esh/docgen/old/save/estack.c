/*	Ausf�hrungsstack
	(c) 1998 Erich Fr�hst�ck
	A-3423 St.Andr�/W�rdern, S�dtirolergasse 17-21/5
*/

#include "eisdoc.h"


ExecStack_t ExecStack = { NULL, NULL };

static ALLOCTAB(buf_stack, 8, sizeof(ExecStack_t));

void PushExecStack(void)
{
	ExecStack_t *x = new_data(&buf_stack);

	x->prev = ExecStack.prev;
	x->tab = rd_refer(ExecStack.tab);
	ExecStack.prev = x;
}

void PopExecStack(void)
{
	if	(ExecStack.prev)
	{
		ExecStack_t *x = ExecStack.prev;
		rd_deref(ExecStack.tab);

		ExecStack.prev = x->prev;
		ExecStack.tab = x->tab;
		del_data(&buf_stack, x);
	}
}
