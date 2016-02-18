# :*: tools for creating makefiles
# :de: Hilfsfunktionen zur Erzeugung von Makefiles
##Depends: shmkmf

mf_srclist

mf_rule -d all files
mf_dir -A files BIN=$TOP/bin
mf_depend -d 'files $(BIN)/pp2dep'

mf_dir SMH=$TOP/lib/shmkmf
mf_dir CFG=$TOP/etc/shmkmf
mf_var -x APP $TOP/lib/efeu

mkprog ()
{
	mf_rule -a $1 $2 "\$(CC) \$(CFLAGS) -o \$@ $2"
}

foreach -m BIN -s c "mkprog \$tg \$src"
foreach -m BIN -s sh 'mf_file -x -s "/:VN:/$EFEU_VERSION/g" $tg $src'
foreach -m SMH -S smh 'mf_file $tg $src'
foreach -m CFG -S cfg 'mf_file $tg $src'

foreach -c $APP -M de -x config -S cnf \
	'mf_file -s "/:VN:/$EFEU_VERSION/g" $tg $src'
foreach -c $APP -M de -x help -S hlp 'mf_file $tg $src'

include efeudoc.smh
