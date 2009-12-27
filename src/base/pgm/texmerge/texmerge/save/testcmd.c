/*	Standardmischbefehle
	(c) 1996 Erich Frühstück
	A-1090 Wien, Währinger Straße 64/6

	Version 1.0
*/

#include "texmerge.h"
#include <memory.h>

#define	STAT_PRE	0	/* Vor Selektion */
#define	STAT_POST	1	/* Nach Selektion */
#define	STAT_END	2	/* Ende der Selektion (nach PP_ELSE) */

typedef struct test_s {
	struct test_s *next;	/* Verweis auch nächsten Eintrag */
	int depth;		/* Falsch - Verschachtelungstiefe */
	int stat;		/* Status */
	int start;		/* Startzeile */
} test_t;

static test_t *cur_test = NULL;

static void test_start(Input_t *in, int flag)
{
	test_t *t;

	t = AllocData("Test", sizeof(test_t));
	t->start = in->line;
	t->next = cur_test;
	cur_test = t;

	if	(flag)
	{
		t->stat = STAT_POST;
		t->depth = 0;
	}
	else
	{
		t->stat = STAT_PRE;
		t->depth = 1;
	}
}


static void test_end(Input_t *in)
{
	if	(cur_test)
	{
		test_t *t = cur_test;
		cur_test = t ? t->next : NULL;
		FreeData("Test", t);
	}
	else	InputError(in, 41);
}


static int arg_value(ArgList_t *arg)
{
	if	(arg == NULL)		return 0;
	if	(arg->dim == 0)		return 0;
	if	(arg->arg[0] == NULL)	return 0;

	return atoi(arg->arg[0]); 
}

void ce_if(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	test_start(in, arg_value(arg));
	SkipLine(in);
}

void ce_elif(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	test_start(in, 1);
	SkipLine(in);
}

void ce_else(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	SkipLine(in);
}

void ce_endif(Command_t *cmd, Input_t *in, Output_t *out, ArgList_t *arg)
{
	test_end(in);
	SkipLine(in);
}
