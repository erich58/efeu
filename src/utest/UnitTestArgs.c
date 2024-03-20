#include <EFEU/UnitTest.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

UnitTestArgs *UnitTestArgs_ptr(UnitTestArgs *args)
{
	static TEST_ARGS_DECL(arg_buf);
	return args ? args : &arg_buf;
}

UnitTestArgs *UnitTestArgs_clear(UnitTestArgs *args)
{
	args = UnitTestArgs_ptr(args);
	args->argc = 0;
	args->argv = realloc(args->argv, 0);
	return args;
}

static int split_args(UnitTestString *buf, const char *def)
{
	if	(!def)
		return 0;

	int n = 0;
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

			UnitTest_putc(*def, buf);
		}
		else if	(*def == '@')
		{
			UnitTest_puts(tmp, buf);
		}
		else if	(quote == 0 && (*def == ' ' || *def == '\t'))
		{
			UnitTest_putc(0, buf);

			while 	(def[1] == ' ' || def[1] == '\t')
				++def;

			n++;
		}
		else
		{
			UnitTest_putc(*def, buf);
		}

		++def;
	}

	UnitTest_putc(0, buf);
	n++;
	return n;
}

UnitTestArgs *UnitTestArgs_init(UnitTestArgs *args, const char *def)
{
	STRING_DECL(buf);

	args = UnitTestArgs_ptr(args);
	args->argc = split_args(&buf, def);
	args->argv = realloc(args->argv,
		(args->argc + 1) * sizeof args->argv[0]);
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
	UnitTest_clear(&buf);
	return args;
}

void UnitTestArgs_info(const char *file, int line, const char *name,
	UnitTestArgs *args)
{
	args = UnitTestArgs_ptr(args);
	UnitTest_info(file, line, "INFO",
		"%s[%d + 1] = {", name ? name : "argv", args->argc);

	for (int i = 0; i <= args->argc; ++i)
		UnitTest_info(file, line, "+", "\t%s,",
			UnitTest_qstr(args->argv[i]));

	UnitTest_info(file, line, "+", "}");
}

UnitTestArgs *UnitTest_mkarg(const char *file, int line, const char *name,
	const char *def)
{
	UnitTestArgs *args = UnitTestArgs_init(NULL, def);
	UnitTestArgs_info(file, line, name, args);
	return args;
}
