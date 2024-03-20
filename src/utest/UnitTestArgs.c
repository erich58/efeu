#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void UnitTestArgs_clear(UnitTestArgs *args)
{
	if	(args)
	{
		args->argc = 0;
		args->argv = realloc(args->argv, 0);
	}
}

UnitTestArgs *UnitTestArgs_init(UnitTestArgs *args, const char *def)
{
	static TEST_ARGS_DECL(arg_buf);

	if	(!args)
		args = &arg_buf;

	if	(!def)
	{
		UnitTestArgs_clear(args);
		return args;
	}

	STRING_DECL(buf);
	args->argc = 0;
	int quote = 0;
	const char *tmp = UnitTest_tmp(NULL);

	while 	(*def == ' ' || *def == '\t')
		++def;

	while (*def != 0)
	{
		if	(*def == '\'')
		{
			quote = !quote;
		}
		else if	(*def == '\\')
		{
			def++;

			if	(*def == 0)
				break;

			UnitTest_putc(*def, &buf);
		}
		else if	(*def == '@')
		{
			UnitTest_puts(tmp, &buf);
		}
		else if	(quote == 0 && (*def == ' ' || *def == '\t'))
		{
			UnitTest_putc(0, &buf);

			while 	(def[1] == ' ' || def[1] == '\t')
				++def;

			args->argc++;
		}
		else
		{
			UnitTest_putc(*def, &buf);
		}

		++def;
	}

	UnitTest_putc(0, &buf);
	args->argc++;
	args->argv = realloc(args->argv, (args->argc + 1) * sizeof args->argv[0]);
	assert(args->argv != NULL);

	size_t p = 0;

	for (int i = 0; i < args->argc; ++i)
	{
		args->argv[i] = buf.data + p;

		while (p < buf.size)
		{
			if	(buf.data[p++] == 0)
				break;
		}
	}

	args->argv[args->argc] = NULL;
	return args;
}
