#include <EFEU/UnitTest.h>
#include <EFEU/CmdPar.h>

static CmdPar *par = NULL;

TEST_SUITE
{
	par = CmdPar_alloc("dummy");
	CmdPar *CmdPar_ptr (CmdPar *ptr);
}
