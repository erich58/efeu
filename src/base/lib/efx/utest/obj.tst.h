#ifndef _utest_obj_tst_h
#define _utest_obj_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/stdtype.h>
#include <EFEU/strbuf.h>

static STRBUF(obj_sbuf, 256);

static char *obj_show(EfiObj *obj)
{
	if	(!obj)
	{
		sb_free(&obj_sbuf);
		return NULL;
	}

	sb_clean(&obj_sbuf);
	IO *io = io_strbuf(&obj_sbuf);
	ShowObj(io, obj); 
	UnrefObj(obj);
	io_close(io);
	return sb_nul(&obj_sbuf);
}

static int obj_type(const char *file, int line,
	EfiObj *obj, const char *value, const char *expr)
{
	int r = 1;
	EfiObj *tg;

	if	(!obj)
	{
		sb_free(&obj_sbuf);
		return 1;
	}

	sb_clean(&obj_sbuf);
	IO *io = io_strbuf(&obj_sbuf);
	io_xprintf(io, "%s -> (%s) %s",
		expr, obj->type->cname, obj->type->name); 
	UnitTest_info(file, line, "# %s", sb_nul(&obj_sbuf));

	io_rewind(io);
	PrintData(io, obj->type, obj->data); 
	r = UnitTest_strcheck(file, line, "==", sb_nul(&obj_sbuf), value, 
		"base", UnitTest_qstr(value));

	tg = ConstObj(obj->type, obj->data);
	io_rewind(io);
	PrintData(io, tg->type, tg->data); 
	r &= UnitTest_strcheck(file, line, "==", sb_nul(&obj_sbuf), value, 
		"copy", UnitTest_qstr(value));
	UnrefObj(tg);

	tg = NewObj(obj->type, NULL);
	io_rewind(io);
	WriteData(obj->type, obj->data, io); 
	io_rewind(io);
	ReadData(tg->type, tg->data, io); 
	io_rewind(io);
	PrintData(io, tg->type, tg->data); 
	r &= UnitTest_strcheck(file, line, "==", sb_nul(&obj_sbuf), value, 
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
	obj_show(NULL)

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
#define	SHOW(term)	obj_show(EvalObj(strterm(term), NULL))

#endif
