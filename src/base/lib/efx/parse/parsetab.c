/*	Parse-Tabelle
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 0.4
*/

#include <EFEU/parsedef.h>
#include <EFEU/stack.h>

static xtab_t *ParseTab = NULL;
static stack_t *ParseStack = NULL;

void *NewParseTab (size_t dim)
{
	return xcreate(dim, skey_cmp);
}

void DelParseTab (void *tab)
{
	xdestroy(tab, NULL);
}

void PushParseTab (void *tab)
{
	pushstack(&ParseStack, ParseTab);
	ParseTab = tab;
}

void *PopParseTab (void)
{
	void *x = ParseTab;
	ParseTab = popstack(&ParseStack, NULL);
	return x;
}

void AddParseDef (ParseDef_t *pdef, size_t dim)
{
	if	(ParseTab == NULL)
		ParseTab = NewParseTab(0);

	xappend(ParseTab, pdef, dim, sizeof(ParseDef_t), XS_REPLACE);
}

ParseDef_t *GetParseDef (const char *name)
{
	ParseDef_t *p;
	stack_t *stack;

	p = skey_find(ParseTab, name);

	for (stack = ParseStack; stack && !p; stack = stack->next)
		p = skey_find(stack->data, name);

	return p;
}

#define	MAX_POS	79

static void print_tab (io_t *io, xtab_t *tab, int depth)
{
	int i, pos;

	io_printf(io, "Schlüsselwörter der Tiefe %d:", depth);
	pos = MAX_POS;

	if	(tab)
	{
		for (i = 0; i < tab->dim; i++)
		{
			char *name = ((ParseDef_t *) tab->tab[i])->name;

			if	(name == NULL)	continue;

			if	(pos + strlen(name) > MAX_POS)
			{
				io_puts("\n\t", io);
				pos = 8;
			}
			else	pos += io_puts(" ", io);

			pos += io_puts(name, io);
		}
	}

	io_putc('\n', io);
}

void ListParseDef (io_t *io)
{
	stack_t *x;
	int depth = 0;

	print_tab(io, ParseTab, depth++);

	for (x = ParseStack; x != NULL; x = x->next)
		print_tab(io, x->data, depth++);
}
