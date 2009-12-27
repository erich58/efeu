/*	Zwischenregister
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "eisdoc.h"

#define	MAXARG 		0x7FFF
#define	LIST_SIZE	8

void InitArgList (ArgList_t *list)
{
	if	(list)
	{
		list->size = 0;
		list->dim = 0;
		list->arg = NULL;
		list->opt = NULL;
	}
}

void ClearArgList (ArgList_t *list)
{
	int i;

	for (i = 0; i < list->dim; i++)
		memfree(list->arg[i]);

	memfree(list->arg);
	memfree(list->opt);
	InitArgList(list);
}


void AddOpt (ArgList_t *list, char *arg)
{
	if	(list)
	{
		memfree(list->opt);
		list->opt = arg;
	}
	else	memfree(arg);
}

void AddArg (ArgList_t *list, char *arg)
{
	if	(list == NULL)
	{
		memfree(arg);
		return;
	}

	if	(list->dim >= list->size)
	{
		size_t size;
		int i;
		char **arg;

		size = list->size + LIST_SIZE;
		arg = memalloc(size * sizeof(char **));

		for (i = 0; i < list->dim; i++)
			arg[i] = list->arg[i];

		list->size = size;
		memfree(list->arg);
		list->arg = arg;
	}

	list->arg[list->dim++] = arg;
}


int SyncArg (ArgList_t *list, int narg)
{
	int n;

	if	(narg < 0)	return 0;

	while (list->dim < narg)
		AddArg(list, NULL);

	for (n = 0; list->dim > narg; n++)
	{
		list->dim--;
		memfree(list->arg[list->dim]);
	}

	return n;
}


void LoadArg (ArgList_t *list, int narg)
{
	int i;
	char *p;

	ParseArg(&list->opt, '[', ']');

	if	(narg < 0)	narg = MAXARG;

	for (i = 0; i < narg; i++)
	{
		if	(!ParseArg(&p, '{', '}'))
			break;

		AddArg(list, p);
	}
}
