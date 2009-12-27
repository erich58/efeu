mf_srclist

mkprog ()
{
	mf_rule -a $1 $2 "\$(CC) \$(CFLAGS) -o \$@ $2"
}

mf_dir BIN=$TOP/bin
mf_dir SMH=$TOP/lib/shmkmf
mf_dir CFG=$TOP/etc/shmkmf

foreach -m BIN -s c 'mkprog $tg $src'
foreach -m BIN -s sh 'mf_file -x $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'
foreach -m BIN -s gencmd "mf_file -x \$tg \$src sh $TOP"
foreach -m CFG -s gencfg "mf_update \$tg.cfg sh \$src $TOP"
