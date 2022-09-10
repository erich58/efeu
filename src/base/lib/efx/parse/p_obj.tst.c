#include <EFEU/UnitTest.h>
#include <EFEU/parsedef.h>
#include <EFEU/printobj.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

static const char *parse(IO *io)
{
	EfiObj *obj = Parse_obj(io, SCAN_ANYTYPE);

	if	(obj)
	{
		io_printf(OUT(), "%s: ", obj->type->name);
		PrintObj(OUT(), obj);
		UnrefObj(obj);
	}
	else
	{
		io_puts("NULL", OUT());
	}

	return utest_out();
}

#define	PARSE(input, val, rem)	\
	IO2STR_FUNCTION(parse, (in), input, val, rem)

extern int PrintFloatPrec;

TEST_SUITE
{
	SetupStd();
	PARSE(" NULL ", "_Ptr_: NULL", " ");
	PARSE(" name ", "_Name_: name", " ");
	PARSE(" 42 ", "int: 42", " ");
	PARSE(" 42l ", "varint: 42", " ");
	PARSE(" 3.5 ", "double: 3.500000000000000", " ");
	PARSE(" 'a' ", "char: 'a'", " ");
	PARSE(" 'ä' ", "wchar_t: 'ä'", " ");
	PARSE(" '€' ", "wchar_t: '€'", " ");
	PARSE(" \"\" ", "str: \"\"", "");
	PARSE(" \"a\" ", "str: \"a\"", "");
	PARSE(" \"a\"\n", "str: \"a\"", "\n");
	PARSE(" \"a\" \"b\".", "str: \"ab\"", ".");
	PARSE(" @", "NULL", "@");
	PARSE(" int ", "Type_t: int", "");
	PARSE(" true ", "bool: true", " ");
	CLEANUP_IN;
	CLEANUP_OUT;
}
