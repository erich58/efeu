#include <EFEU/UnitTest.h>
#include <GUI/ETK.h>

#if	HAS_TCL

static const char *parse(ETK *etk, const char *def)
{
	static EfiObj *obj = NULL;

	UnrefObj(obj);
	obj = NULL;

	if	(!def)
		return NULL;

	IO *in = io_cstr(def);
	obj = ETK_parse(etk, in);
	io_close(in);
	EfiExpr *expr = obj->data;
	return Val_str(expr->list->obj->data);
}

TEST_SUITE
{
	SetupETK();
	ETK *etk = ETK_create();
	ASSERT(etk != NULL);
	EXPECT_STR(parse(etk, " :: name eod"), "name");
	EXPECT_STR(parse(etk, " :: name [ x ] eod"), "name");
	EXPECT_STR(parse(etk, " :: vec ( x ) eod"), "vec( x )");
	EXPECT_STR(parse(etk, " a b c \neod"), "a b c ");
	EXPECT_STR(parse(etk, " : a b c \neod"), ": a b c ");
	EXPECT_STR(parse(etk, " [ a b c ] eod"), " a b c ");
	EXPECT_STR(parse(etk, " [ a b \\"), " a b \\");
	EXPECT_STR(parse(etk, " [ a b \\\nc d"), " a b c d");
	EXPECT_STR(parse(etk, " \\[ a b c ] \neod"), "\\[ a b c ] ");
	EXPECT_STR(parse(etk, " a [ b { c ( d ) e } f ] g \neod"),
		"a [ b { c ( d ) e } f ] g ");
	EXPECT_STR(parse(etk, " [ a ( b ] c ) d ] \neod"),
		" a ( b ] c ) d ");
	EXPECT_STR(parse(etk, " [ a \" ( b \" ] c ) d ] \neod"),
		" a \" ( b \" ");

	streval("tcl::a");
	streval("tcl");
	streval("tcl []");
	parse(etk, NULL);
}

#else
TEST_SUITE
{
	PRINT_LABEL("Tcl/Tk not availabel");
}
#endif
