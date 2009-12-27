/*	Stack von unten nach oben durchwandern
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/


#include <EFEU/oldstack.h>


int walkstack(oldstack_t *stack, visit_t visit)
{
	oldstack_t *x;
	int n;

	if	(stack == NULL)
	{
		return 0;
	}

	n = 0;
	x = stack->prev;

	do
	{
		if	(visit && !(*visit)(x->value))
		{
			break;
		}

		x = x->prev;
		n++;
	}
	while	(x != stack->prev);

	return n;
}
