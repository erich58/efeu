/*	Such nach Eintrag in Tabellenstack
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/oldstack.h>


void *stacksearch(oldstack_t *stack, void *key)
{
	void *p;
	oldstack_t *x;

	if	(stack == NULL)	return NULL;

	x = stack;

	do
	{
		if	((p = xsearch(x->value, key, XS_FIND)) != NULL)
		{
			return p;
		}
		else	x = x->next;
	}
	while	(x != stack);

	return NULL;
}
