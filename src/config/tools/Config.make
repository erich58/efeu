mf_srclist

mf_rule -d all files
mf_dir -A files BIN=$TOP/bin
mf_depend -d 'files $(BIN)/pp2dep'

mf_dir SMH=$TOP/lib/shmkmf
mf_dir CFG=$TOP/etc/shmkmf

mkprog ()
{
	mf_rule -a $1 $2 "\$(CC) \$(CFLAGS) -o \$@ $2"
}

foreach -m BIN -s c "mkprog \$tg \$src"
foreach -m BIN -s sh 'mf_file -x $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'
foreach -m CFG -S cfg 'mf_file $tg $src'

