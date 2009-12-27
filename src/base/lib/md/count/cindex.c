/*	Indexliste aus Formatdefinition generieren
	(c) 1994 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 2
*/

#include <EFEU/mdmat.h>
#include <EFEU/mdcount.h>
#include <EFEU/object.h>

static int idx = 0;

static Var_t vartab[] = {
	{ "n", &Type_int, &idx }
};


char **md_cindex(MdClass_t *class)
{
	char *fmt;
	char **p;
	int j;

	if	(class == NULL || class->dim == 0)
	{
		return NULL;
	}

/*	Indexliste generieren
*/
	p = ALLOC(class->dim, char *);
	fmt = class->idxfmt;

	if	(fmt == NULL && class->dim == 1)
	{
		p[0] = mstrcpy(class->name);
		return p;
	}

	reg_cpy(1, class->name);

	if	(fmt && *fmt == '=')
	{
		for (j = 0, fmt++; j < class->dim; j++)
			p[j] = msprintf(fmt, j);
	}
	else if	(fmt && *fmt == '+')
	{
		for (j = 0, fmt++; j < class->dim; j++)
			p[j] = msprintf(fmt, j + 1);
	}
	else if	(fmt && *fmt == '!')
	{
		PushVarTab(NULL, NULL);
		AddVar(NULL, vartab, tabsize(vartab));

		for (idx = 0; idx < class->dim; idx++)
			p[idx] = parsub(fmt + 1);

		PopVarTab();
	}
	else
	{
		char **ilist;
		int n;

		n = strsplit(class->idxfmt, "%s", &ilist);
		n = min(n, class->dim);

		for (j = 0; j < n; j++)
			p[j] = mstrcpy(ilist[j]);

		memfree(ilist);

		for (; j < class->dim; j++)
		{
			reg_set(2, msprintf("%d", j));
			p[j] = parsub("$1[$2]");
		}
	}

	return p;
}
