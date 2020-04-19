/*	Test für Testabdeckung durch obj_type()
*/

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/strbuf.h>

#include "utest/obj.tst.h"

static STRBUF(buf, 256);

/* Test Datentyp: const String */

#define	VAL(x)	((const char **)(x))[0]

static size_t read_data (const EfiType *type, void *data, IO *io)
{
	VAL(data) = io_getstr(io);
	sb_printf(&buf, "read %s\n", VAL(data));
	return type->size;
}

static size_t write_data (const EfiType *type, const void *data, IO *io)
{
	io_putstr(VAL(data), io);
	sb_printf(&buf, "write %s\n", VAL(data));
	return type->size;
}

static int print_data (const EfiType *type, const void *data, IO *io)
{
	sb_printf(&buf, "print %s\n", VAL(data));
	return io_printf(io, "|%s|", VAL(data));
}

static void clean_data(const EfiType *st, void *data, int mode)
{
	sb_printf(&buf, "clean %s\n", VAL(data));
}

static void copy_data(const EfiType *st, void *tg, const void *src)
{
	sb_printf(&buf, "copy %s -> %s\n", VAL(src), VAL(tg));
	VAL(tg) = VAL(src);
}

static EfiType test_type = COMPLEX_TYPE("test", "const char *",
	sizeof(char *), 0,
	read_data, write_data, print_data,
	NULL, NULL, clean_data, copy_data, TYPE_MALLOC);


TEST_SUITE
{
	TEST_OBJ_TYPE(NewPtrObj(&test_type, "A"), "|A|");
	EXPECT_STR(sb_nul(&buf), 
		"print A\n"
		"copy A -> (null)\n"
		"print A\n"
		"clean A\n"
		"write A\n"
		"clean (null)\n"
		"read A\n"
		"print A\n"
		"clean A\n"
		"clean A\n"
	);
	sb_free(&buf);
	CLEANUP_OBJ;
}
