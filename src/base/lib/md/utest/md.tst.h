/*	Demo-Datenmatrix für Tests
*/

#ifndef	_utest_md_tst_h
#define	_utest_md_tst_h 1
#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>

mdmat *mdtest_create(const char *def);
const char *mdtest_show(mdmat *md);

static void mdtest_set_value(mdaxis *axis, char *data, int value)
{
	if	(axis)
	{
		value *= 10;

		for (size_t n = 0; n < axis->dim; n++)
		{
			++value;
			mdtest_set_value(axis->next, data, value);
			data += axis->size;
		}
	}
	else
	{
		*((int *) data) = value;
	}
}

mdmat *mdtest_create(const char *def)
{
	mdmat *md = md_create(&Type_int, def);
	mdtest_set_value(md->axis, md->data, 0);
	return md;
}

const char *mdtest_show(mdmat *md)
{
	if	(!md)	return NULL;

	IO *out = io_fopen(UnitTest_tmp(".md"), "w");
	md_mark(md, "#-1");
	io_puts(md_showflag(md, 1), out);
	md_print(out, md, "nohead");
	md_mark(md, NULL);
	return UnitTest_content(".md");
}

#endif
