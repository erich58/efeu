#include <EFEU/UnitTest.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

typedef struct ParEntry ParEntry;

struct ParEntry {
	char *name;
	char *value;
	ParEntry *next;
};

static ParEntry *par_root = NULL;

static ParEntry *find_entry(const char *name)
{
	ParEntry *p = par_root;

	while (p && strcmp(p->name, name) != 0)
		p = p->next;

	return p;
}

static void del_entry(const char *name)
{
	ParEntry **p = &par_root;

	while (*p != NULL)
	{
		ParEntry *x = *p;

		if	(!name || strcmp(x->name, name) == 0)
		{
			*p = x->next;
			free(x->name);
			free(x->value);
			free(x);

		}
		else
		{
			p = &(x->next);
		}
	}
}

void UnitTest_tplpar(const char *name, const char *value)
{
	if	(value)
	{
		ParEntry *p = find_entry(name);

		if	(p)
		{
			free(p->value);
			p->value = strdup(value);
		}
		else if	(name)
		{
			p = malloc(sizeof *p);
			p->name = strdup(name);
			p->value = strdup(value);
			p->next = par_root;
			par_root = p;
		}
	}
	else
	{
		del_entry(name);
	}
}

static const char *get_par(const char *name)
{
	if	(*name == '?')
	{
		static STRING_DECL(buf);
		buf.size = 0;

		ParEntry *p = par_root;

		while (p != NULL)
		{
			if	(buf.size)
				UnitTest_putc(' ', &buf);

			UnitTest_puts(p->name , &buf);
			UnitTest_putc('=', &buf);
			UnitTest_puts(p->value , &buf);
			p = p->next;
		}

		UnitTest_putc(0, &buf);
		return buf.data;
	}

	ParEntry *p = find_entry(name);
	return p ? p->value : name;
}

const char *UnitTest_tpl(const char *tpl)
{
	if	(tpl)
	{
		static STRING_DECL(buf);
		buf.size = 0;

		while (*tpl)
		{
			if	(*tpl == '\\')
			{
				++tpl;

				if	(*tpl)
					UnitTest_putc(*tpl, &buf);
				else
					break;
			}
			else if	(*tpl == '@')
			{
				UnitTest_puts(UnitTest_tmp(NULL), &buf);
			}
			else if	(*tpl == '{')
			{
				STRING_DECL(name);
				tpl++;

				while (*tpl && *tpl != '}')
				{
					UnitTest_putc(*tpl, &name);
					++tpl;
				}

				UnitTest_puts(get_par(UnitTest_str(&name)),
					&buf);
				UnitTest_free(&name);

				if	(!*tpl)
					break;
			}
			else
			{
				UnitTest_putc(*tpl, &buf);
			}

			++tpl;
		}

		UnitTest_putc(0, &buf);
		return buf.data;
	}

	return NULL;
}
