/*	Rotieren der Stackelemente
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/oldstack.h>


void rotstack(oldstack_t **stack, int direc)
{
	if	(stack != NULL && *stack != NULL)
	{
		for ( ; direc > 0; direc--)
			*stack = (*stack)->next;

		for ( ; direc < 0; direc++)
			*stack = (*stack)->prev;
	}
}


/*	Vertauschen zweier Elemente im Stack
*/

void exchange(oldstack_t **stack)
{
	if	(stack != NULL && *stack != NULL)
	{
		void *x;

		x = (*stack)->value;
		(*stack)->value = (*stack)->next->value;
		(*stack)->next->value = x;
	}
}
