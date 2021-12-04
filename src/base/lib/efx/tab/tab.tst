#!/usr/bin/env .build/script.tst

Tabular tab = Tabular(NULL, NULL);
EXPECT_STR(tab.ident, "\
Tabular: size=1, cols=1, entry=0\n\
\n\
");

tab = Tabular("c|c||");
puts("1", tab.io);
tab.entry();
puts("2", tab.io);
tab.endline();
tab.hline(1);
puts("3", tab.io);
tab.entry();
puts("4", tab.io);
tab.endline()();
tab.hline(2);

EXPECT_STR(tab.ident, "\
Tabular: size=9, cols=2, entry=4\n\
 1 | 2 #\n\
--+--#\n\
 3 | 4 #\n\
==#==#\n\
");
