/*	Stack von oben nach unten durchwandern
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/oldstack.h>

int rwalkstack(oldstack_t *stack, visit_t visit)
{
	oldstack_t *x;
	int n;

	if	(stack == NULL)
	{
		return 0;
	}

	n = 0;
	x = stack;

	do
	{
		if	(visit && !(*visit)(x->value))
		{
			break;
		}

		x = x->next;
		n++;
	}
	while	(x != stack);

	return n;
}
