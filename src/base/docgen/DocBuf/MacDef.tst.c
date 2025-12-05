#include <EFEU/UnitTest.h>
#include <EFEU/mstring.h>
#include "DocBuf.h"
#include <EFEU/utest/in.tst.h>

static const char *macinfo(void (*func)(StrBuf *buf))
{
	static STRBUF(buf, 1024);
	sb_clear(&buf);
	func(&buf);
	return sb_cstr(&buf);
}


TEST_SUITE
{
	MacDef_clean();
	EXPECT_STR(MacDef_name(), NULL);

	MacDef_add(IN("Name(arg) \\\n"
		"First line, \\\n"
		"Last line\\\n"
		"/*Description*/\n"
	));
	MacDef_add(IN("Uncommented 42\n"));
	MacDef_add(IN("Simple 12345 //Simple macro\n"));
	EXPECT_STR(MacDef_name(), "Name");

	EXPECT_STR(macinfo(MacDef_synopsis),
		"\\index{Name}(arg)\\br\n"
		"\\index{Uncommented}\\br\n"
		"\\index{Simple}\\br\n"
		"\n"
	);
	EXPECT_STR(macinfo(MacDef_source),
		"#define Name(arg) First line, Last line/* Description*/\n"
		"#define Uncommented 42/* */\n"
		"#define Simple 12345 /* Simple macro*/\n"
	);

	EXPECT_STR(macinfo(MacDef_list),
		"\\ttitem Name(arg)\n"
		"Description\n"
		"\\ttitem Simple\n"
		"Simple macro\n"
		"\\endlist\n"
	);

	CLEANUP_IN;
}
