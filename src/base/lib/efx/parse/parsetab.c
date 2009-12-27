/*
Parse-Tabelle

$Copyright (C) 1994 Erich Frühstück
This file is part of EFEU.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; see the file COPYING.Library.
If not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
*/

#include <EFEU/parsedef.h>
#include <EFEU/stack.h>

static NameKeyTab *ParseTab = NULL;
static Stack *ParseStack = NULL;

void *NewParseTab (size_t dim)
{
	return nkt_create("Parse", dim, NULL);
}

void DelParseTab (void *tab)
{
	rd_deref(tab);
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

void AddParseDef (EfiParseDef *pdef, size_t dim)
{
	if	(ParseTab == NULL)
		ParseTab = NewParseTab(0);

	for (; dim--; pdef++)
		nkt_insert(ParseTab, pdef->name, pdef);
}

EfiParseDef *GetParseDef (const char *name)
{
	EfiParseDef *p;
	Stack *stack;

	p = nkt_fetch(ParseTab, name, NULL);

	for (stack = ParseStack; stack && !p; stack = stack->next)
		p = nkt_fetch(stack->data, name, NULL);

	return p;
}

#define	MAX_POS	79

static void print_tab (IO *io, NameKeyTab *tab, int depth)
{
	int n, pos;

	io_xprintf(io, "Schlüsselwörter der Tiefe %d:", depth);
	pos = MAX_POS;

	if	(tab)
	{
		NameKeyEntry *ptr = tab->tab.data;

		for (n = tab->tab.used; n-- > 0; ptr++)
		{
			if	(ptr->name == NULL)	continue;

			if	(pos + strlen(ptr->name) > MAX_POS)
			{
				io_puts("\n\t", io);
				pos = 8;
			}
			else	pos += io_puts(" ", io);

			pos += io_puts(ptr->name, io);
		}
	}

	io_putc('\n', io);
}

void ListParseDef (IO *io)
{
	Stack *x;
	int depth = 0;

	print_tab(io, ParseTab, depth++);

	for (x = ParseStack; x != NULL; x = x->next)
		print_tab(io, x->data, depth++);
}
