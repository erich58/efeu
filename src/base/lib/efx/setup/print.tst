#!/usr/bin/env .build/script.tst

#define EXPECT_POUT(__e, __r) \
fprint(OUT(), __e); \
UnitTest::strcheck(__ID__, "==", UnitTest::out_data(), __r, \
sprintf("output((%s) %s)", typeof(__e), # __e), # __r)

OUT() << fprint;
MATCH_OUT(".*int fprint \(IO, .*");

EXPECT(field_width == 0);
EXPECT(float_prec == 5);
EXPECT(PrintTypeVerbosity == 3);

EXPECT_STR(fmt_str, "%#*s");
EXPECT_STR(fmt_float, "%#*.*g");
EXPECT_STR(fmt_int, "%*i");
EXPECT_STR(fmt_unsigned, "%*u");
EXPECT_STR(fmt_size, "%*lu");
EXPECT_STR(fmt_int64, "%*lli");
EXPECT_STR(fmt_uint64, "%*llu");

EXPECT_STR(PrintListBegin, "{");
EXPECT_STR(PrintListDelim, ", ");
EXPECT_STR(PrintListEnd, "}");

EXPECT_STR(str(2.5), "2.5000");

TEST_EXPR(SetFloatPrec("0f"));
EXPECT(float_prec == 0);
EXPECT_STR(fmt_float, "%*.*f");
EXPECT_POUT(12.5, "13");

TEST_EXPR(SetFloatPrec("#5"));
EXPECT(float_prec == 5);
EXPECT_STR(fmt_float, "%#*.*g");
EXPECT_POUT(1.7, "1.7000");

EXPECT_POUT(!true, "false");
EXPECT_POUT(5, "5");
EXPECT_POUT(5u, "5");
EXPECT_POUT(5l, "5");
EXPECT_POUT(5lu, "5");
EXPECT_POUT('a', "a");
EXPECT_POUT('ä', "\303\244");
EXPECT_POUT(L'ä', "\303\244");
EXPECT_POUT("abc", "\"abc\"");
EXPECT_POUT(x, "(_undef_) x");
EXPECT_POUT(NULL, "NULL");
EXPECT_POUT(1:3, "{1, 2, 3}");
EXPECT_POUT(int, "int");
EXPECT_POUT([a], "[a]");
EXPECT_POUT(xdiv, "double xdiv (double, double)");

int a = 3;
EXPECT_POUT(a.operator"++", "(3) int & int::operator\"++\" & ()\n");

VirFunc vfunc;
EXPECT_POUT(vfunc, "NULL");
ObjFunc ofunc;
EXPECT_POUT(ofunc, "NULL");

OUT() << 'x' << 5 << "abc" << '<' << NULL << '>';
EXPECT_OUT("x5abc<>");
fprint(OUT(), 1, "a");
EXPECT_OUT("1 \"a\"");

VarTab vtab = VarTab("tab");
int vtab::a = 5;
EXPECT_POUT(vtab, "tab = {\n\ta\n}");
int v[] = 1 : 3;
EXPECT_POUT(v, "{1, 2, 3}");
EXPECT_POUT(iostd, "IO: <stdin,stdout>");
EXPECT_POUT(Object(42), "(Object) 42");

/*
        -:  466:
        -:  467:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Name_)", fprint_name },
        -:  468:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _MemberName_)", fprint_mname },
        -:  469:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _ScopeName_)", fprint_sname },
        -:  471:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, restricted _Ptr_)", fprint_ptr },
        -:  472:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, _Ref_)", fprint_ref },
        -:  474:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, Object)", fprint_obj },
        -:  475:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, EfiVec)", fprint_vec },
        -:  477:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, VarTab)", fprint_vtab },
        -:  481:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, .)", fprint_any },
        -:  482:	{ FUNC_VIRTUAL, &Type_int, "fprint (IO, ...)", fprint_va_list },
        -:  483:
        -:  484:	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, char)", put_char },
        -:  485:	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, str)", put_str },
        -:  486:	{ FUNC_VIRTUAL, &Type_io, "operator<< (IO, .)", put_obj },
*/
