#ifndef _utest_obj_tst_h
#define _utest_obj_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/strbuf.h>

static int obj_type(const char *file, int line,
	EfiObj *obj, const char *value, const char *expr)
{
	static STRBUF(buf, 256);
	int r = 1;
	EfiObj *tg;

	if	(!obj)
	{
		sb_free(&buf);
		return 1;
	}

	sb_clean(&buf);
	IO *io = io_strbuf(&buf);
	io_xprintf(io, "%s -> (%s) %s",
		expr, obj->type->cname, obj->type->name); 
	UnitTest_info(file, line, "# %s", sb_nul(&buf));

	io_rewind(io);
	PrintData(io, obj->type, obj->data); 
	r = UnitTest_strcheck(file, line, "==", sb_nul(&buf), value, 
		"base", UnitTest_qstr(value));

	tg = ConstObj(obj->type, obj->data);
	io_rewind(io);
	PrintData(io, tg->type, tg->data); 
	r &= UnitTest_strcheck(file, line, "==", sb_nul(&buf), value, 
		"copy", UnitTest_qstr(value));
	UnrefObj(tg);

	tg = NewObj(obj->type, NULL);
	io_rewind(io);
	WriteData(obj->type, obj->data, io); 
	io_rewind(io);
	ReadData(tg->type, tg->data, io); 
	io_rewind(io);
	PrintData(io, tg->type, tg->data); 
	r &= UnitTest_strcheck(file, line, "==", sb_nul(&buf), value, 
		"out-in", UnitTest_qstr(value));
	UnrefObj(tg);
	UnrefObj(obj);
	io_close(io);
	return r;
}

#define	TEST_OBJ_TYPE(obj, ref)	\
	obj_type(__FILE__, __LINE__, obj, ref, # obj)

#define	TEST_TERM_TYPE(term, ref)	\
	obj_type(__FILE__, __LINE__, EvalObj(strterm(term), NULL), ref, # term)

#define	CLEANUP_OBJ \
	obj_type(__FILE__, __LINE__, NULL, NULL, NULL)

#define	EXPECT_ITERM(term, value)	\
	UnitTest_result(__FILE__, __LINE__, \
	Obj2varint(strterm(term)) == value, "%s -> %lld", \
	UnitTest_qstr(term), value) 

#define	EXPECT_UTERM(term, value)	\
	UnitTest_result(__FILE__, __LINE__, \
	Obj2varsize(strterm(term)) == value, "%s -> %#llx", \
	UnitTest_qstr(term), value) 

#define	EXPECT_DTERM(term, value)	\
	UnitTest_result(__FILE__, __LINE__, \
	Obj2double(strterm(term)) == value, "%s -> %#f", \
	UnitTest_qstr(term), value) 

#define	EVAL(term)	Obj2str(strterm(term))

#endif
