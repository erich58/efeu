#include <EFEU/UnitTest.h>
#include <GUI/tk_config.h>

#if	HAS_TCL

#include <tcl.h>
#include <tk.h>

static Tcl_Interp *interp = NULL;

static const char *ObjType(Tcl_Obj *obj)
{
	static STRING_DECL(buf);

	UnitTest_clear(&buf);

	if	(obj->typePtr)
		UnitTest_puts(obj->typePtr->name, &buf);

	int len = 0;
	char *p = Tcl_GetStringFromObj(obj, &len);
	UnitTest_puts(": ", &buf);
	UnitTest_puts(p, &buf);
	UnitTest_putc('\n', &buf);
	Tcl_DecrRefCount(obj);
	return UnitTest_str(&buf);
}

TEST_SUITE
{
	interp = Tcl_CreateInterp();
	LMATCH_STR(ObjType(Tcl_NewBooleanObj(1)), "int: 1\n");
	LMATCH_STR(ObjType(Tcl_NewIntObj(~0u >> 1)), "int: \n");
	LMATCH_STR(ObjType(Tcl_NewLongObj(~0ul >> 1)), "int: \n");
	LMATCH_STR(ObjType(Tcl_NewDoubleObj(2.5)), "double: \n");
	LMATCH_STR(ObjType(Tcl_NewStringObj("abc", -1)), ": abc\n");
	LMATCH_STR(ObjType(Tcl_NewStringObj("abc", 2)), ": ab\n");
}

#else
TEST_SUITE
{
//	PRINT_LABEL("Tcl/Tk not availabel");
	UnitTest_result(__FILE__, __LINE__, 0, "Tcl/Tk not availabel");
}
#endif
