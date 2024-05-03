#include <EFEU/UnitTest.h>
#include <EFEU/mdmat.h>
#include <EFEU/mdlabel.h>

TEST_SUITE
{
	MdAxisLabel *lbl;
	SetupMdMat();
	EXPECT(set_label(NULL) == NULL);

	EXPECT_PTR(lbl = init_label("x", "a"));
	EXPECT_STR(lbl->name, "x");
	EXPECT(lbl->next == NULL);
	del_label(lbl);

	EXPECT_PTR(lbl = init_label("x", "a.b"));
	EXPECT_STR(lbl->name, "x1");
	EXPECT_STR(lbl->next->name, "x2");
	EXPECT(lbl->next->next == NULL);
	del_label(lbl);

	EXPECT_PTR(lbl = set_label("x y"));
	EXPECT_STR(lbl->name, "x");
	EXPECT_STR(lbl->next->name, "y");
	EXPECT(lbl->next->next == NULL);

	IO *tmp = io_tmpbuf(0);
	EXPECT_INT(save_label(tmp, lbl, NULL), 1);

	EXPECT_INT(save_label(tmp, lbl, "a1.b1"), 0);
	EXPECT_INT(save_label(tmp, lbl, "a1.b2"), 0);
	EXPECT_INT(save_label(tmp, lbl, "a1.b3"), 0);
	EXPECT_INT(save_label(tmp, lbl, "a2.b1"), 0);
	EXPECT_INT(save_label(tmp, lbl, "a2.b2"), 0);
	EXPECT_INT(save_label(tmp, lbl, "a2.b3"), 0);
	io_rewind(tmp);
	EXPECT_INT(io_getval(tmp, 2), 0);
	EXPECT_INT(io_getval(tmp, 2), 0);
	EXPECT_INT(io_getval(tmp, 2), 0);
	EXPECT_INT(io_getval(tmp, 2), 1);
	EXPECT_INT(io_getval(tmp, 2), 0);
	EXPECT_INT(io_getval(tmp, 2), 2);
	EXPECT_INT(io_getval(tmp, 2), 1);
	EXPECT_INT(io_getval(tmp, 2), 0);
	EXPECT_INT(io_getval(tmp, 2), 1);
	EXPECT_INT(io_getval(tmp, 2), 1);
	EXPECT_INT(io_getval(tmp, 2), 1);
	EXPECT_INT(io_getval(tmp, 2), 2);

	StrPool *sbuf = NewStrPool();
	mdaxis *axis = label2axis(sbuf, lbl);
	EXPECT_STR(StrPool_get(sbuf, axis->i_name), "x");
	EXPECT_STR(StrPool_get(sbuf, axis->next->i_name), "y");

	del_label(lbl);
}
