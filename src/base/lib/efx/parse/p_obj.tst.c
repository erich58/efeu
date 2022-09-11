#include <EFEU/UnitTest.h>
#include <EFEU/parsedef.h>
#include <EFEU/printobj.h>
#include "utest/term.tst.h"

#define	PARSE_OBJ(in)	obj_to_str(Parse_obj(in, SCAN_ANYTYPE))

#define	PARSE(input, val, rem)	\
	EXPECT_IO2STR(PARSE_OBJ, input, val, rem)

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
	CLEANUP_TERM;
}
