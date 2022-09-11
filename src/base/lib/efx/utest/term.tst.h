/*	Test auf Termauswertung mit Freigabe
*/

#ifndef	_utest_term_tst_h
#define	_utest_term_tst_h	1

#include <EFEU/UnitTest.h>
#include <EFEU/object.h>
#include <EFEU/printobj.h>
#include <EFEU/memalloc.h>
#include "utest/in.tst.h"
#include "utest/out.tst.h"

static const char *obj_to_str(EfiObj *obj)
{
	if	(obj)
	{
		io_printf(OUT(), "%s: ", obj->type->name);
		PrintObj(OUT(), obj);
		UnrefObj(obj);
	}
	else
	{
		io_puts("NULL", OUT());
	}

	return utest_out();
}

static const char *utest_term(IO *io)
{
	return obj_to_str(EvalObj(Parse_term(io, 0), NULL));
}

static void utest_term_clean(void)
{
	utest_term(IN(0));
	utest_out_clean();
}

#define	EXPECT_TERM(input, val, rem) \
	EXPECT_IO2STR(utest_term, input, val, rem)

#define	CLEANUP_TERM utest_term_clean()

#endif
