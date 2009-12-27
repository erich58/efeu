/*	Wert auf Stack legen
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/oldstack.h>
#include <EFEU/memalloc.h>


static ALLOCTAB(stacktab, 32, sizeof(oldstack_t));


void pushstack(oldstack_t **stack, const void *value)
{
	oldstack_t *x;

	if	(stack == NULL)
	{
		return;
	}

	x = (oldstack_t *) new_data(&stacktab);
	x->value = (void *) value;

	if	(*stack == NULL)
	{
		x->prev = x;
		x->next = x;
	}
	else
	{
		x->next = *stack;
		x->prev = x->next->prev;
		x->prev->next = x;
		x->next->prev = x;
	}

	*stack = x;
}


/*	Element am Kopf des Stacks ablegen
*/

void topstack(oldstack_t **stack, const void *value)
{
	if	(stack != NULL)
	{
		pushstack(stack, value);
		*stack = (*stack)->next;
	}
}


/*	Wert aus Stack extrahieren
*/

void *popstack(oldstack_t **stack)
{
	oldstack_t *root;
	void *x;

	if	(stack == NULL || *stack == NULL)
	{
		return NULL;
	}

	root = *stack;

	if	(root->next != root)
	{
		root->next->prev = root->prev;
		root->prev->next = root->next;
		*stack = root->next;
	}
	else	*stack = NULL;

	x = root->value;
	del_data(&stacktab, root);
	return x;
}


void clrstack(oldstack_t **stack, void (*action) (void *))
{
	if	(stack == NULL)
	{
		;
	}
	else if	(action)
	{
		while (*stack != NULL)
			(*action)(popstack(stack));
	}
	else
	{
		while (*stack != NULL)
			popstack(stack);
	}
}
