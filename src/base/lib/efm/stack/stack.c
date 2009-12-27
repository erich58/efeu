/*	Stackstruktur
	(c) 1996 Erich Fr�hst�ck
	A-1090 Wien, W�hringer Stra�e 64/6

	Version 0.4
*/

#include <EFEU/stack.h>
#include <EFEU/memalloc.h>

#define	STACK_BSIZE	63	/* Blockgr��e f�r Stacksegmente */

static ALLOCTAB(stack_tab, STACK_BSIZE, sizeof(stack_t));

void pushstack (stack_t **ptr, void *data)
{
	stack_t *par = new_data(&stack_tab);
	par->next = *ptr;
	par->data = data;
	*ptr = par;
}

void *popstack (stack_t **ptr, void *data)
{
	if	(*ptr)
	{
		stack_t *par = *ptr;
		*ptr = par->next;
		data = par->data;
		del_data(&stack_tab, par);
	}

	return data;
}
