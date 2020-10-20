#!/usr/bin/env .build/script.tst

EXPECT_REPR(Pixmap, "Pixmap");
Pixmap p = Pixmap(3, 2, 0x414243);
EXPECT_REPR(p, "Pixmap: 3x2x3");
EXPECT(p.cols == 3);
EXPECT(p.rows == 2);
p.save(UnitTest::tmp("pix"));
EXPECT_CONTENT("pix", "P6\n3 2\n255\nABCABCABCABCABCABC");
p = NULL;
EXPECT_REPR(p, "NULL");
p = LoadPixmap(UnitTest::tmp("pix"));
EXPECT_REPR(p, "Pixmap: 3x2x3");
