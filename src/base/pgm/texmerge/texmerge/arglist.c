/*	Zwischenregister
	(c) 1997 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"

#define	MAXARG 		0x7FFF
#define	LIST_SIZE	4

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
		FreeData("STR", list->arg[i]);

	FreeData("ARGPTR", list->arg);
	FreeData("STR", list->opt);
	InitArgList(list);
}


void AddOpt (ArgList_t *list, char *arg)
{
	if	(list)
	{
		FreeData("STR", list->opt);
		list->opt = arg;
	}
	else	FreeData("STR", arg);
}

void AddArg (ArgList_t *list, char *arg)
{
	if	(list == NULL)
	{
		FreeData("STR", arg);
		return;
	}

	if	(list->dim >= list->size)
	{
		size_t size;
		int i;
		char **arg;

		size = list->size + LIST_SIZE;
		arg = AllocData("ARGPTR", size * sizeof(char **));

		for (i = 0; i < list->dim; i++)
			arg[i] = list->arg[i];

		list->size = size;
		FreeData("ARGPTR", list->arg);
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
		FreeData("STR", list->arg[list->dim]);
	}

	return n;
}


int LoadArg (ArgList_t *list, Input_t *in, int narg)
{
	int i, n;
	char *arg;

	n = 0;
	list->opt = GetArg(in, '[', ']');

	if	(GetArgStatus)	n++;

	if	(narg < 0)	narg = MAXARG;

	for (i = 0; i < narg; i++)
	{
		arg = GetArg(in, '{', '}');

		if	(!GetArgStatus)
			break;

		AddArg(list, arg);
		n++;
	}

	return n;
}
