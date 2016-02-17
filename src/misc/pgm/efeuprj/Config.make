# :*: EFEU specific tools
# :de: EFEU spezifische Werkzeuge
##Depends: tools

mf_srclist

mkprog ()
{
	mf_rule -a $1 $2 "\$(CC) \$(CFLAGS) -o \$@ $2"
}

mf_dir BIN=$TOP/bin
mf_dir SMH=$TOP/lib/shmkmf
mf_dir CFG=$TOP/etc/shmkmf
mf_var -x APP $TOP/lib/efeu

foreach -m BIN -s c 'mkprog $tg $src'
foreach -m BIN -s sh 'mf_file -x -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'
foreach -m BIN -s gencmd "mf_file -x \$tg \$src 'VN=\"$EFEU_VERSION\" sh' $TOP"
foreach -m CFG -s gencfg "mf_update \$tg.cfg 'VN=\"$EFEU_VERSION\" sh' \$src $TOP"

foreach -c $APP -M de -x config -S cnf \
	'mf_file -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -c $APP -M de -x help -S hlp 'mf_file $tg $src'
