CC=efeucc
include cc.smh

mf_srclist
foreach -s c 'mf_cc -o $tg $src -lefm'
