#include <EFEU/UnitTest.h>
#include "src2doc.h"

TEST_SUITE
{
	STRBUF(buf, 64);
	SrcData data;
	SrcData_init(&data, NULL);

	SrcData_copy(&data, &buf, "Name");
	EXPECT_STR(sb_cstr(&buf), "");

	sb_puts("Some text\n", &data.buf);
	SrcData_copy(&data, &buf, "Name");
	EXPECT_STR(sb_cstr(&buf), "Some text\n");

	sb_puts("$include /dev/null\n", &data.buf);
	SrcData_copy(&data, &buf, "Name");
	EXPECT_STR(sb_cstr(&buf), "Some text\n");
}
