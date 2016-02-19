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
mf_var -x APP $TOP/lib/efeu

foreach -m BIN -s c 'mkprog $tg $src'
foreach -m BIN -s sh 'mf_file -x -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'

foreach -c $APP -M de -x config -S cnf \
	'mf_file -s "/:VN:/$EFEU_VERSION/" $tg $src'
foreach -c $APP -M de -x help -S hlp 'mf_file $tg $src'

include efeudoc.smh
