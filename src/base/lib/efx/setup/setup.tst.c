#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/Resource.h>
#include <EFEU/math.h>
#include <EFEU/CmdPar.h>
#include "utest/mstr.tst.h"
#include "utest/out.tst.h"
#include "utest/obj.tst.h"
#include <math.h>

TEST_DECL(base)
{
	EfiObj *obj;
	void *data;

	TEST_OBJ_TYPE(bool2Obj(0), "false");
	TEST_OBJ_TYPE(bool2Obj(1), "true");
	TEST_OBJ_TYPE(char2Obj('a'), "'a'");
	TEST_OBJ_TYPE(ucs2Obj(L'€'), "'€'");
	TEST_OBJ_TYPE(str2Obj(mstrcpy("abc")), "\"abc\"");
	EXPECT_MSTR(obj_show(char2Obj('a')), "a");
	EXPECT_MSTR(obj_show(ucs2Obj(L'Ä')), "Ä");
	EXPECT_MSTR(SHOW("'a'"), "a");
	EXPECT_MSTR(SHOW("L'Ä'"), "Ä");

	obj = NewObj(&Type_ptr, NULL);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("NULL");
	data = (void *) 0x1234;
	CopyData(obj->type, obj->data, &data);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("_Ptr_(0x1234)");
	EXPECT(WriteData(obj->type, obj->data, OUT()) == 0);
	EXPECT_OUT("");
	UnrefObj(obj);

	obj = NewObj(&Type_void, NULL);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("void()");
	UnrefEval(obj);

	obj = str2Obj(NULL);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("NULL");
	EXPECT(ReadData(obj->type, obj->data, ionull) == 0);
	EXPECT_STR(Val_str(obj->data), NULL);
	IO *io = io_strbuf(0);
	io_putc(0, io);
	io_rewind(io);
	EXPECT(ReadData(obj->type, obj->data, io) == 1);
	EXPECT_STR(Val_str(obj->data), NULL);
	UnrefObj(obj);

	obj = NewPtrObj(&Type_name, NULL);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("operator\"\"");
	data = (void *) "++";
	CopyData(obj->type, obj->data, &data);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("operator\"++\"");
	data = (void *) "abc";
	CopyData(obj->type, obj->data, &data);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("abc");
	obj = EvalObj(obj, NULL);
	EXPECT(obj->type == &Type_undef);
	EXPECT_MSTR(Obj2str(obj), "(_undef_) abc");

	obj = type2Obj(&Type_int);
	EXPECT_MSTR(obj_show(RefObj(obj)), "int");
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("int");
	obj = obj2Obj(obj);
	EXPECT(obj->type == &Type_obj);
	PrintData(OUT(), obj->type, obj->data);
	EXPECT_OUT("int");
	EXPECT_MSTR(obj_show(obj), "(Object) int");

	EXPECT_MSTR(EVAL("[3*5]"), "[15]");
	EXPECT_MSTR(EVAL("(void)5"), NULL);
	EXPECT_MSTR(SHOW("(VirFunc) xdiv"), "double xdiv (double, double)\n");
	MATCH_MSTR(EVAL("(VirFunc) int::operator++"), "\\(VirFunc\\) ");
	MATCH_MSTR(EVAL("VarTab(\"x\")"), "\\(VarTab\\) ");
	EXPECT_ITERM("!iostd", 0);
	EXPECT_ITERM("iostd == ioerr", 0);
	EXPECT_ITERM("iostd != ioerr", 1);

	PushVarTab(NULL, NULL);
	EXPECT_MSTR(EVAL("str a = \"abc\""), "abc");
	EXPECT_MSTR(EVAL("free(a)"), NULL);
	EXPECT_MSTR(EVAL("a"), NULL);
	EXPECT_ITERM("Object o = 2", 2);
	PopVarTab();

	SetResource("ResourceData", "Data");
	EXPECT_MSTR(EVAL("ResourceData"), "Data");
}

TEST_DECL(stdint)
{
	TEST_OBJ_TYPE(int2Obj(-12345), "-12345");
	EXPECT_UTERM("(unsigned) 12345", 12345);
	EXPECT_MSTR(EVAL("-12345"), "-12345");
	EXPECT_MSTR(SHOW("-12345"), "-12345");

	TEST_OBJ_TYPE(uint2Obj(12345), "12345u");
	EXPECT_ITERM("(int)    ~0u", -1);
	EXPECT_ITERM("(int8_t) ~0u", -1);
	EXPECT_ITERM("(int16_t)~0u", -1);
	EXPECT_ITERM("(int32_t)~0u", -1);
	EXPECT_ITERM("(int64_t)~0u", 0xFFFFFFFFll);
	EXPECT_UTERM("(unsigned)~0u", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) ~0u", 0xFF);
	EXPECT_UTERM("(uint16_t)~0u", 0xFFFF);
	EXPECT_UTERM("(uint32_t)~0u", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)~0u", 0xFFFFFFFFull);
	EXPECT_MSTR(EVAL("12345u"), "12345");
	EXPECT_MSTR(SHOW("12345u"), "12345");

	TEST_TERM_TYPE("(int8_t)0x80", "-128");
	EXPECT_ITERM("(int)    (int8_t)0x80", -128);
	EXPECT_ITERM("(int8_t) (int8_t)0x80", -128);
	EXPECT_ITERM("(int16_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(int32_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(int64_t)(int8_t)0x80", -128);
	EXPECT_ITERM("(unsigned)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint8_t) (int8_t)0x7f", 127);
	EXPECT_ITERM("(uint16_t)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint32_t)(int8_t)0x7f", 127);
	EXPECT_ITERM("(uint64_t)(int8_t)0x7f", 127);

	TEST_TERM_TYPE("(uint8_t) ~0", "255");
	EXPECT_ITERM("(int)    (uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int8_t) (uint8_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int32_t)(uint8_t)~0", 0xFF);
	EXPECT_ITERM("(int64_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(unsigned)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint8_t) (uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint32_t)(uint8_t)~0", 0xFF);
	EXPECT_UTERM("(uint64_t)(uint8_t)~0", 0xFF);

	TEST_TERM_TYPE("(int16_t) -128", "-128");
	EXPECT_ITERM("(int)    (int16_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int16_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int16_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int16_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int16_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int16_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int16_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int16_t)0x7FFF", 0x7FFF);
	
	TEST_TERM_TYPE("(uint16_t) 12345", "12345");
	EXPECT_ITERM("(int)    (uint16_t)~0", 0xFFFF);
	EXPECT_ITERM("(int8_t) (uint16_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint16_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint16_t)~0", 0xFFFF);
	EXPECT_ITERM("(int64_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(unsigned)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint8_t) (uint16_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint16_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint64_t)(uint16_t)~0", 0xFFFF);

	TEST_TERM_TYPE("(int32_t) -128", "-128");
	EXPECT_ITERM("(int)    (int32_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int32_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int32_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int32_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int32_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int32_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int32_t)0x7FFF", 0x7FFF);
	EXPECT_MSTR(EVAL("(int32_t) -12345"), "-12345");
	EXPECT_MSTR(SHOW("(int32_t) -12345"), "-12345");

	TEST_TERM_TYPE("(uint32_t) 128", "128");
	EXPECT_ITERM("(int)    (uint32_t)~0", -1);
	EXPECT_ITERM("(int8_t) (uint32_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint32_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint32_t)~0", -1);
	EXPECT_ITERM("(int64_t)(uint32_t)~0", 0xFFFFFFFFll);
	EXPECT_UTERM("(unsigned)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) (uint32_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint32_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)(uint32_t)~0", 0xFFFFFFFFull);
	EXPECT_MSTR(EVAL("(uint32_t) 12345"), "12345");
	EXPECT_MSTR(SHOW("(uint32_t) 12345"), "12345");

	TEST_TERM_TYPE("(int64_t) -128", "-128");
	EXPECT_ITERM("(int)    (int64_t)-128", -128);
	EXPECT_ITERM("(int8_t) (int64_t)-128", -128);
	EXPECT_ITERM("(int16_t)(int64_t)-128", -128);
	EXPECT_ITERM("(int32_t)(int64_t)-128", -128);
	EXPECT_ITERM("(int64_t)(int64_t)-128", -128);
	EXPECT_UTERM("(unsigned)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint8_t) (int64_t)0x7FFF", 0xFF);
	EXPECT_UTERM("(uint16_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint32_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_UTERM("(uint64_t)(int64_t)0x7FFF", 0x7FFF);
	EXPECT_MSTR(EVAL("(int64_t) -1234567890"), "-1234567890");
	EXPECT_MSTR(SHOW("(int64_t) -1234567890"), "-1234567890");

	TEST_TERM_TYPE("(uint64_t) 128", "128");
	EXPECT_ITERM("(int)    (uint64_t)~0", -1);
	EXPECT_ITERM("(int8_t) (uint64_t)~0", -1);
	EXPECT_ITERM("(int16_t)(uint64_t)~0", -1);
	EXPECT_ITERM("(int32_t)(uint64_t)~0", -1);
	EXPECT_ITERM("(int64_t)(uint64_t)~0", -1);
	EXPECT_UTERM("(unsigned)(uint64_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) (uint64_t)~0", 0xFF);
	EXPECT_UTERM("(uint16_t)(uint64_t)~0", 0xFFFF);
	EXPECT_UTERM("(uint32_t)(uint64_t)~0", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)(uint64_t)~0", 0xFFFFFFFFFFFFFFFFull);
	EXPECT_MSTR(EVAL("(uint64_t) 1234567890"), "1234567890");
	EXPECT_MSTR(SHOW("(uint64_t) 1234567890"), "1234567890");

	TEST_OBJ_TYPE(varint2Obj(-12345), "-12345");
	EXPECT_ITERM("(int)    -128l", -128);
	EXPECT_ITERM("(int8_t) -128l", -128);
	EXPECT_ITERM("(int16_t)-128l", -128);
	EXPECT_ITERM("(int32_t)-128l", -128);
	EXPECT_ITERM("(int64_t)-128l", -128);
	EXPECT_UTERM("(unsigned)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint8_t) 0x7FFFl", 0xFF);
	EXPECT_UTERM("(uint16_t)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint32_t)0x7FFFl", 0x7FFF);
	EXPECT_UTERM("(uint64_t)0x7FFFl", 0x7FFF);
	EXPECT_MSTR(EVAL("-1234567890l"), "-1234567890");

	TEST_OBJ_TYPE(varsize2Obj(42), "42");
	EXPECT_ITERM("(int)    ~0ul", -1);
	EXPECT_ITERM("(int8_t) ~0ul", -1);
	EXPECT_ITERM("(int16_t)~0ul", -1);
	EXPECT_ITERM("(int32_t)~0ul", -1);
	EXPECT_ITERM("(int64_t)~0ul", -1);
	EXPECT_UTERM("(unsigned)~0ul", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint8_t) ~0ul", 0xFF);
	EXPECT_UTERM("(uint16_t)~0ul", 0xFFFF);
	EXPECT_UTERM("(uint32_t)~0ul", 0xFFFFFFFFull);
	EXPECT_UTERM("(uint64_t)~0ul", 0xFFFFFFFFFFFFFFFFull);
	EXPECT_MSTR(EVAL("1234567890ul"), "1234567890");
}

TEST_DECL(stdfloat)
{
	TEST_TERM_TYPE("float(3.5)", "3.5000000");
	TEST_OBJ_TYPE(double2Obj(3.5), "3.500000000000000");
	SetFloatPrec("#4g");
	EXPECT_MSTR(EVAL("12.5"), "12.50");
	EXPECT_MSTR(SHOW("12.5"), "12.50");
	SetFloatPrec("0f");
	EXPECT_MSTR(SHOW("12.5"), "13");
	SetFloatPrec("#5");
}

TEST_DECL(type)
{
	EXPECT_MSTR(SHOW("str.name"), "\"str\"");
	EXPECT_MSTR(SHOW("str.cname"), "\"char *\"");
	EXPECT_MSTR(SHOW("str.desc"), "NULL");
	EXPECT_ITERM("str.size", sizeof(char *));
	EXPECT_ITERM("str.recl", 0);
	EXPECT_ITERM("int.default", 0);
	EXPECT_MSTR(SHOW("str.base"), "_Ptr_");
	EXPECT_MSTR(SHOW("str.flags"), "4");
	EXPECT_MSTR(SHOW("int.flags"), "0");
	EXPECT_MSTR(SHOW("Func.var"), "Func = {\n\ttype\n}");
	UnitTest_stdout("stdout");
	EXPECT_MSTR(EVAL("bool.info(\"print\")"), NULL);
	fflush(stdout);
	EXPECT_CONTENT("stdout",
		"name = bool\n"
		"\n"
		"default = false\n"
		"\n"
		"cname = \"int\"\n"
		"size = 4\n"
		"recl = 4\n"
		"\n"
		"io:\tprint\n"
		"copy:\tmemcpy\n"
		"clean:\tmemset\n"
		"\n"
		"conv\n"
		"create\n"
		"par\n"
		"var\n"
	);
}

TEST_DECL(unix)
{
	TEST_TERM_TYPE("mode_t(0100777)", "-rwxrwxrwx");
}

TEST_SUITE
{
	SetupStd();
	SetupMath();
	TEST_RUN(base);
	TEST_RUN(stdint);
	TEST_RUN(stdfloat);
	TEST_RUN(type);
	TEST_RUN(unix);
	CLEANUP_MSTR;
	CLEANUP_OUT;
	CLEANUP_OBJ;
}
